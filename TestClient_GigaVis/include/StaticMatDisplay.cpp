// StaticMatDisplay.cpp : implementation file
//

#include "pch.h"
#include "StaticMatDisplay.h"
#include <opencv2/opencv.hpp>

using namespace cv;

// CStaticMatDisplay
Mat	m_Image	;	//	이미지

IMPLEMENT_DYNAMIC(CStaticMatDisplay, CStatic)

CStaticMatDisplay::CStaticMatDisplay(BOOL bUseTracker, BOOL bWheelZoom, BOOL bImageDrag)
{
	m_Image.release()			;
	m_pBitmap		=	NULL	;

	ZeroMemory( &m_ptImagePos, sizeof(POINT) )		;
	ZeroMemory( &m_ptLBDown, sizeof(POINT) )		;
	ZeroMemory( &m_ptBeforImagePos, sizeof(POINT) )	;

	m_fScale		=	1.0f		;
	m_iScale		=	1			;
	m_bImageMoving	=	FALSE		;
	m_bWheelZoom	=	bWheelZoom	;
	m_bImageDrag = bImageDrag;
	m_bUseTracker = bUseTracker;

	SetActiveImageRect(bUseTracker);
}

CStaticMatDisplay::~CStaticMatDisplay()
{
	DeleteAll();
	m_Image.release()	;

	if( m_pBitmap != NULL )
	{
		m_pBitmap->DeleteObject()	;
		delete	m_pBitmap			;
		m_pBitmap	=	NULL		;
	}

	ZeroMemory( &m_ptImagePos, sizeof(POINT) )		;
	ZeroMemory( &m_ptLBDown, sizeof(POINT) )		;
	ZeroMemory( &m_ptBeforImagePos, sizeof(POINT) )	;

	m_fScale		=	1.0f	;
	m_iScale		=	1		;
	m_bImageMoving	=	FALSE	;
}


BEGIN_MESSAGE_MAP(CStaticMatDisplay, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


VOID	CStaticMatDisplay::CalcAndDisplayImage()
{
	if( m_pBitmap == NULL )
		return	;

	//	비트맵 검은색으로 채움
	CClientDC	dc( this )	;
	CDC			MemDC		;
	CBitmap		*pOldBitmap				;
	CBrush		NewBrush( RGB(0,0,0) )	;

	CRect		rt	;
	GetClientRect( &rt )	;

	MemDC.CreateCompatibleDC( &dc )	;

	pOldBitmap	=	MemDC.SelectObject( m_pBitmap )		;
	MemDC.FillRect( &rt, &NewBrush )					;
	NewBrush.DeleteObject()								;
	MemDC.SelectObject( pOldBitmap )					;

	if( m_Image.data == NULL )
	{
		Invalidate()	;
		return			;
	}

	long		WindowWidth, WindowHeight, ImageWidth, ImageHeight	;
	RECT		rtImageDrawArea	;
	POINT		ptDrawImageLT, ptDrawWindowLT	;
	SIZE		DrawImageSize, DrawWindowSize 	;
	BITMAPINFO	bi								;

	WindowWidth			=	rt.Width()			;
	WindowHeight		=	rt.Height()			;
	ImageWidth			=	m_Image.cols		;
	ImageHeight			=	m_Image.rows		;

	m_WinRect = rt;

	memset( &bi, 0, sizeof(BITMAPINFO) )							;
	bi.bmiHeader.biSize			=	sizeof( BITMAPINFOHEADER )		;
	bi.bmiHeader.biWidth		=	ImageWidth						;
	bi.bmiHeader.biHeight		=	-ImageHeight					;
	bi.bmiHeader.biPlanes		=	1								;
	bi.bmiHeader.biBitCount		=	(WORD)(m_Image.elemSize()*8)	;
	bi.bmiHeader.biCompression	=	BI_RGB							;

	//	이미지 좌표를 기준으로 화면에 그려질 영역을 계산
	rtImageDrawArea.left	=	((long)(ImageWidth / 2) - m_ptImagePos.x) - (long)(((float)(WindowWidth / 2) / m_fScale) + 0.5f)	;
	rtImageDrawArea.top		=	((long)(ImageHeight / 2) - m_ptImagePos.y) - (long)(((float)(WindowHeight / 2) / m_fScale) + 0.5f)	;
	rtImageDrawArea.right	=	rtImageDrawArea.left + (long)(((float)WindowWidth / m_fScale) + 0.5f)								;
	rtImageDrawArea.bottom	=	rtImageDrawArea.top + (long)(((float)WindowHeight / m_fScale) + 0.5f)								;

	//	이미지가 그려질 영역이 이미지를 벗어나 있으면 추가 처리 진행 안함
	if( rtImageDrawArea.left >= ImageWidth || rtImageDrawArea.right < 0 || rtImageDrawArea.top >= ImageHeight || rtImageDrawArea.bottom < 0 )
	{
		Invalidate()	;
		return			;
	}

	if( rtImageDrawArea.left < 0 )
	{
		ptDrawWindowLT.x	=	(long)(abs(rtImageDrawArea.left) * m_fScale)	;
		ptDrawImageLT.x		=	0												;
	}
	else
	{
		ptDrawWindowLT.x	=	0						;
		ptDrawImageLT.x		=	rtImageDrawArea.left	;
	}

	if( rtImageDrawArea.right > ImageWidth )
		DrawImageSize.cx	=	ImageWidth - ptDrawImageLT.x	;
	else
		DrawImageSize.cx	=	rtImageDrawArea.right - ptDrawImageLT.x + 1	;

	DrawWindowSize.cx	=	(long)(DrawImageSize.cx * m_fScale)	;

/*
	if( rtImageDrawArea.top < 0 )
	{
		ptDrawWindowLT.y	=	(long)(abs(rtImageDrawArea.top) * m_fScale)	;
		ptDrawImageLT.y		=	0											;
	}
	else
	{
		ptDrawWindowLT.y	=	0					;
		ptDrawImageLT.y		=	rtImageDrawArea.top	;
	}

	if( rtImageDrawArea.bottom > ImageHeight )
		DrawImageSize.cy	=	ImageHeight - ptDrawImageLT.y	;
	else
		DrawImageSize.cy	=	rtImageDrawArea.bottom - ptDrawImageLT.y	;

	DrawWindowSize.cy	=	(long)(DrawImageSize.cy * m_fScale)	;
*/

	//	위, 아래의 경우 이미지 상의 그릴 좌표를 자르지 않고 윈도우 상의 그릴 좌표를 바깥쪽으로 이동 시켜야 정상적으로 그려짐
	if( rtImageDrawArea.top < 0 )
	{
		ptDrawWindowLT.y	=	(long)(abs(rtImageDrawArea.top) * m_fScale)	;
	}
	else
	{
		ptDrawWindowLT.y	=	(long)(-rtImageDrawArea.top * m_fScale)	;
	}

	ptDrawImageLT.y		=	0									;
	DrawImageSize.cy	=	ImageHeight							;
	DrawWindowSize.cy	=	(long)(DrawImageSize.cy * m_fScale)	;


	pOldBitmap	=	MemDC.SelectObject( m_pBitmap )	;
	MemDC.SetStretchBltMode( COLORONCOLOR )			;
	StretchDIBits( MemDC.GetSafeHdc(), ptDrawWindowLT.x, ptDrawWindowLT.y, DrawWindowSize.cx, DrawWindowSize.cy,
		ptDrawImageLT.x, ptDrawImageLT.y, DrawImageSize.cx, DrawImageSize.cy, m_Image.data, &bi, DIB_RGB_COLORS, SRCCOPY )	;
	MemDC.SelectObject( pOldBitmap )	;
	MemDC.DeleteDC()					;

	m_lZoomStartX = 0;
	m_lZoomStartY = 0;
	m_lZoomEndX = DrawImageSize.cx;
	m_lZoomEndY = DrawImageSize.cy;

	Invalidate()	;
}

BOOL CStaticMatDisplay::SetImage(Mat img)
{
	if (img.rows == 0 || img.cols == 0)
		return FALSE;

	
	img.copyTo(m_Image);
	if (m_Image.channels() == 1)
	{
		cv::cvtColor( m_Image, m_Image, COLOR_GRAY2BGRA )	;	//	단순히 보여주는 용이므로 4Byte Align을 맞추기 위하여 BGR가 아닌 BGRA로 함
	}
	else if( m_Image.channels() == 3 )
	{
		//cv::cvtColor( m_Image, m_Image, COLOR_BGR2RGB)	;	//	단순히 보여주는 용이므로 4Byte Align을 맞추기 위하여 BGR가 아닌 BGRA로 함
	}
	CalcAndDisplayImage();
}
//
//BOOL	CStaticMatDisplay::SetImage( IplImage* pImage )
//{
//	if( pImage == NULL )
//		return	FALSE	;
//
//	if (m_lpImage)		cvReleaseImage(&m_lpImage);
//	m_lpImage = NULL;
//	
//	m_lpImage = cvCreateImage(cvSize(pImage->width, pImage->height), IPL_DEPTH_8U, 1);
//
//	if(pImage->nChannels == 3)
//	{
//		cvCvtColor(pImage, m_lpImage, CV_BGR2GRAY);
//	}
//	else
//	{
//		cvCopy(pImage, m_lpImage);
//	}
//
//	CRect		rt	;
//	GetClientRect( &rt )	;
//	m_WinRect = rt;
//	m_lZoomEndX = pImage->width;
//	m_lZoomStartX = 0;
//	m_lZoomEndY = pImage->height;
//	m_lZoomStartX = 0;
//
//	m_Image.release()				;
//	m_Image	=	cvarrToMat(pImage )	;
//	/*
//	if( m_Image.channels() == 1 )
//	{
//		cv::cvtColor( m_Image, m_Image, COLOR_GRAY2BGRA )	;	//	단순히 보여주는 용이므로 4Byte Align을 맞추기 위하여 BGR가 아닌 BGRA로 함
//	}
//	else if( m_Image.channels() == 3 )
//	{
//		cv::cvtColor( m_Image, m_Image, COLOR_BGR2BGRA )	;	//	단순히 보여주는 용이므로 4Byte Align을 맞추기 위하여 BGR가 아닌 BGRA로 함
//	}
//*/
//	CalcAndDisplayImage()		;
//
//	return	TRUE	;
//}

VOID	CStaticMatDisplay::ZoomIn()
{
	if( m_iScale < 32 )
	{
		if( m_fScale < 1 )
		{
			if( ((float)1 / (float)abs(m_iScale)) != m_fScale )
			{
				m_fScale	=	((float)1 / (float)abs(m_iScale))	;
				CalcAndDisplayImage()			;
				return	;
			}
		}
		else
		{
			if( (float)m_iScale != m_fScale )
			{
				m_fScale	=	(float)m_iScale	;
				CalcAndDisplayImage()			;
				return	;
			}
		}

		if( abs(m_iScale) == 1 || m_iScale == 0 )
			m_iScale	=	2	;
		else if( m_iScale == -2 )
			m_iScale	=	1	;
		else
			m_iScale++	;

		if( m_iScale > 0 )
			m_fScale	=	(float)m_iScale	;
		else
			m_fScale	=	((float)1 / (float)abs(m_iScale))	;

		CalcAndDisplayImage()			;
	}
}

VOID	CStaticMatDisplay::ZoomOut()
{
	if( m_iScale > -32 )
	{
		if( abs(m_iScale) == 1 || m_iScale == 0 )
			m_iScale	=	-2	;
		else
			m_iScale--	;

		if( m_iScale > 0 )
			m_fScale	=	(float)m_iScale	;
		else
			m_fScale	=	((float)1 / (float)abs(m_iScale))	;

		CalcAndDisplayImage()			;
	}
}

VOID	CStaticMatDisplay::Fit()
{
	if( m_Image.data == NULL )
		return	;

	long		WindowWidth, WindowHeight, ImageWidth, ImageHeight	;
	float		VertRate, HorzRate	;
	CRect		rt		;
	GetClientRect( rt )	;

	WindowWidth			=	rt.Width()			;
	WindowHeight		=	rt.Height()			;
	ImageWidth			=	m_Image.cols		;
	ImageHeight			=	m_Image.rows		;

	HorzRate	=	(float)WindowWidth	/	(float)ImageWidth	;
	VertRate	=	(float)WindowHeight	/	(float)ImageHeight	;

	if( HorzRate < VertRate )
	{
		m_fScale	=	HorzRate	;
	}
	else
	{
		m_fScale	=	VertRate	;
	}

	if( m_fScale < 1 )
	{
		m_iScale	=	(long)((float)1 / m_fScale)	;
		m_iScale	*=	-1	;
	}
	else
	{
		m_iScale	=	(long)m_fScale	;
	}

	if( abs(m_iScale) > 32 )	//	32배 이상 안되도록 강제 설정
	{
		if( m_iScale < 0 )
		{
			m_iScale	=	-32	;
			m_fScale	=	((float)1 / (float)abs(m_iScale))	;
		}
		else
		{
			m_iScale	=	32	;
			m_fScale	=	(float)m_iScale	;
		}
	}

	m_ptImagePos.x	=	m_ptImagePos.y	=	0	;
	CalcAndDisplayImage()	;
}

VOID	CStaticMatDisplay::OriginalScale()
{
	if( m_iScale == 1 && m_ptImagePos.x == 0 && m_ptImagePos.y == 0 )
		return	;

	m_fScale	=	1.0f	;
	m_iScale	=	1		;
	m_ptImagePos.x	=	m_ptImagePos.y	=	0	;
	CalcAndDisplayImage()	;
}

// CStaticMatDisplay message handlers




void CStaticMatDisplay::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages
	RECT		rt			;
	GetClientRect( &rt )	;
	if( m_pBitmap == NULL )
	{
		rt.right	+=	rt.right % 2	;	//	짝수 맞춤
		rt.bottom	+=	rt.bottom % 2	;	//	짝수 맞춤
		RECT	WindowRect	;
		GetWindowRect( &WindowRect )	;
		GetParent()->ScreenToClient( &WindowRect )	;
		MoveWindow( WindowRect.left, WindowRect.top, rt.right, rt.bottom )	;

		m_pBitmap		=	new	CBitmap()								;
		m_pBitmap->CreateCompatibleBitmap( &dc, rt.right, rt.bottom )	;

//		ModifyStyle( 0, SS_NOTIFY )	;	//	마우스 이벤트를 받기 위해서 스타일 추가
	}


	CDC			MemDC		;
	CBitmap		*pOldBitmap	;

	MemDC.CreateCompatibleDC( &dc )	;
	pOldBitmap	=	MemDC.SelectObject( m_pBitmap )					;
	dc.BitBlt( 0, 0, rt.right, rt.bottom, &MemDC, 0, 0, SRCCOPY )	;
	MemDC.SelectObject( pOldBitmap )								;
	MemDC.DeleteDC()	;
}


void CStaticMatDisplay::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( m_bImageDrag && m_Image.data != NULL )
	{
		m_ptLBDown.x		=	point.x			;
		m_ptLBDown.y		=	point.y			;
		m_ptBeforImagePos.x	=	m_ptImagePos.x	;
		m_ptBeforImagePos.y	=	m_ptImagePos.y	;
		m_bImageMoving		=	TRUE			;
	}

		CalcAndDisplayImage();

	CStatic::OnLButtonDown(nFlags, point);
}


void CStaticMatDisplay::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_bImageMoving	=	FALSE	;

	

	CalcAndDisplayImage();


	CStatic::OnLButtonUp(nFlags, point);
}


void CStaticMatDisplay::OnMButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	Fit()	;

	

	CStatic::OnMButtonDblClk(nFlags, point);
}

BOOL CStaticMatDisplay::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	if(!m_bWheelZoom)
		return CStatic::OnMouseWheel(nFlags, zDelta, pt);

	if( zDelta < 0 )
		ZoomIn()	;
	else
		ZoomOut()	;

	return CStatic::OnMouseWheel(nFlags, zDelta, pt);
}


void CStaticMatDisplay::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if( m_bImageDrag && m_bImageMoving && nFlags == MK_LBUTTON && m_Image.data != NULL )
	{
		float	diff_x, diff_y	;
		diff_x	=	(m_ptLBDown.x - point.x) / m_fScale		;
		diff_y	=	(m_ptLBDown.y - point.y) / m_fScale		;

		m_ptImagePos.x	=	(long)(m_ptBeforImagePos.x - diff_x)	;
		m_ptImagePos.y	=	(long)(m_ptBeforImagePos.y - diff_y)	;
		CalcAndDisplayImage()	;
	}

	m_nMouseCurX = (int)((float)m_ptImagePos.x + ((float)point.x) / m_fScale);
	m_nMouseCurY = (int)((float)m_ptImagePos.y + ((float)point.y) / m_fScale);

	if(m_nMouseCurX < 0)
	{
		m_nMouseCurX = 0;
	}

	if(m_nMouseCurY < 0)
	{
		m_nMouseCurY = 0;
	}

	/*if(m_nMouseCurX > m_lpImage->width - 1)
	{
		m_nMouseCurX = m_lpImage->width - 1;
	}

	if(m_nMouseCurY > m_lpImage->height - 1)
	{
		m_nMouseCurY = m_lpImage->height - 1;
	}*/

	//m_byMouseCurVal = m_lpImage->imageData[m_nMouseCurX + (m_lpImage->widthStep * m_nMouseCurY )];


	CStatic::OnMouseMove(nFlags, point);
}


void CStaticMatDisplay::Draw()
{
	CRect rect;
	CDC *pDC;
	pDC=GetDC();

	CPen pen,*oldPen;
	CBrush brush,*oldBrush;

	GetClientRect(&rect);

	//빈공간 채우기
	pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	oldPen=pDC->SelectObject(&pen);
	brush.CreateSolidBrush(RGB(0, 0, 0));
	oldBrush=pDC->SelectObject(&brush);
	
	rect.left = m_WinRect.right;
	rect.top = m_WinRect.top;

	pDC->Rectangle(&rect);
	
	pDC->SelectObject(oldPen);
	pen.DeleteObject();
	pDC->SelectObject(oldBrush);
	brush.DeleteObject();

	ReleaseDC(pDC);
}


//사각형 영역 추가
void CStaticMatDisplay::AddTracker(int type, CRect rect, COLORREF color, char *cap)
{

	/*C_CtrlTracker tracker;
	int idx;
	double rect_l, rect_t, rect_r, rect_b;

	if(m_bActive)
	{
		m_curColor = color;
		switch(type)
		{
			case 0:
			case 1:
				tracker = C_CtrlTracker(this, type, 4, 2);
				tracker.SetColor(m_curColor);
				
				rect_l = rect.left;
				rect_t = rect.top;
				rect_r = rect.right;
				rect_b = rect.bottom;

				rect_l = (rect_l - m_lZoomStartX) * ((double)m_WinRect.Width()/(m_lZoomEndX - m_lZoomStartX));
				rect_t = (rect_t - m_lZoomStartY) * ((double)m_WinRect.Height()/(m_lZoomEndY - m_lZoomStartY));

				rect_r = (rect_r - m_lZoomStartX) * ((double)m_WinRect.Width()/(m_lZoomEndX - m_lZoomStartX));
				rect_b = (rect_b - m_lZoomStartY) * ((double)m_WinRect.Height()/(m_lZoomEndY - m_lZoomStartY));

				tracker.SetRect(rect_l, rect_t, rect_r, rect_b);
				tracker.SetZoom(m_lZoomStartX, m_lZoomStartY, m_lZoomEndX, m_lZoomEndY);
				tracker.SetCaption(cap);
				idx = m_Tracker.GetCount();
				m_Tracker.AddTail(tracker);
				break;
			default:
				AfxMessageBox(_T("Error : invalid index!"));
				break;
		}

	}*/

}


//모든 영역 삭제
void CStaticMatDisplay::DeleteAll()
{
	/*if(m_bActive && ( !m_Tracker.IsEmpty() ) )
	{
		m_Tracker.RemoveAll();
		m_iActiveIdx = -1;
	}*/
}

//영역 삭제(생성해 있지 않으면 FALSE 반환)
BOOL CStaticMatDisplay::DeleteTracker(int idx)
{
	/*C_CtrlTracker tracker;

	if(m_bActive && ( !m_Tracker.IsEmpty()) )
	{
		POSITION pos = m_Tracker.FindIndex(idx);
		if(NULL != pos)
		{
			m_Tracker.RemoveAt(pos);
			m_iActiveIdx = -1;

			return TRUE;
		}
	}*/

	return FALSE;
}

//사각형 영역 얻기(생성해 있지 않으면 FALSE 반환)
BOOL CStaticMatDisplay::GetTracker(int idx, CRect *rect)
{
	//C_CtrlTracker tracker;
	//double rect_l, rect_t, rect_r, rect_b;

	//if(!m_Tracker.IsEmpty())
	//{
	//	POSITION pos = m_Tracker.FindIndex(idx);
	//	if(NULL != pos)
	//	{
	//		tracker = m_Tracker.GetAt(pos);
	//		switch(tracker.GetType())
	//		{
	//			case 0:
	//			case 1:
	//				tracker.GetRect(&rect_l, &rect_t, &rect_r, &rect_b);

	//				/*
	//				rect_l = (rect_l - m_lZoomStartX) / ((double)m_WinRect.Width()/(m_lZoomEndX - m_lZoomStartX));
	//				rect_t = (rect_t - m_lZoomStartY) / ((double)m_WinRect.Height()/(m_lZoomEndY - m_lZoomStartY));

	//				rect_r = (rect_r - m_lZoomStartX) / ((double)m_WinRect.Width()/(m_lZoomEndX - m_lZoomStartX));
	//				rect_b = (rect_b - m_lZoomStartY) / ((double)m_WinRect.Height()/(m_lZoomEndY - m_lZoomStartY));
	//				*/

	//				//2090924
	//				rect_l = m_lZoomStartX + ( ((double)rect_l)  * ((m_lZoomEndX - m_lZoomStartX) / (double)m_WinRect.Width()));
	//				rect_t = m_lZoomStartY + ( ((double)rect_t)  * ((m_lZoomEndY - m_lZoomStartY) / (double)m_WinRect.Height()));
	//				rect_r = m_lZoomStartX + ( ((double)rect_r)  * ((m_lZoomEndX - m_lZoomStartX) / (double)m_WinRect.Width()));
	//				rect_b = m_lZoomStartY + ( ((double)rect_b)  * ((m_lZoomEndY - m_lZoomStartY) / (double)m_WinRect.Height()));


	//				rect_l = rect_l + 0.5;
	//				rect_t = rect_t + 0.5;
	//				rect_r = rect_r + 0.5;
	//				rect_b = rect_b + 0.5;

	//				*rect = CRect(rect_l, rect_t, rect_r, rect_b);
	//				break;
	//			default:
	//				AfxMessageBox(_T("Error : invalid index!"));
	//				break;
	//		}
	//		
	//		return TRUE;
	//	}
	//}

	return FALSE;
}


//사각형 영역 설정(생성해 있지 않으면 FALSE 반환)
BOOL CStaticMatDisplay::SetTracker(int idx, CRect rect)
{
	/*C_CtrlTracker tracker;

	double rect_l, rect_t, rect_r, rect_b;

	if(!m_Tracker.IsEmpty())
	{
		POSITION pos = m_Tracker.FindIndex(idx);
		if(NULL != pos)
		{
			tracker = m_Tracker.GetAt(pos);
			switch(tracker.GetType())
			{
				case 0:
				case 1:
				rect_l = rect.left;
				rect_t = rect.top;
				rect_r = rect.right;
				rect_b = rect.bottom;

				rect_l = (rect_l - m_lZoomStartX) * ((double)m_WinRect.Width()/(m_lZoomEndX - m_lZoomStartX));
				rect_t = (rect_t - m_lZoomStartY) * ((double)m_WinRect.Height()/(m_lZoomEndY - m_lZoomStartY));

				rect_r = (rect_r - m_lZoomStartX) * ((double)m_WinRect.Width()/(m_lZoomEndX - m_lZoomStartX));
				rect_b = (rect_b - m_lZoomStartY) * ((double)m_WinRect.Height()/(m_lZoomEndY - m_lZoomStartY));

				tracker.SetRect(rect_l, rect_t, rect_r, rect_b);
				tracker.SetZoom(m_lZoomStartX, m_lZoomStartY, m_lZoomEndX, m_lZoomEndY);

				idx = m_Tracker.GetCount();
				m_Tracker.AddTail(tracker);
					break;
				default:
					AfxMessageBox(_T("Error : invalid index!"));
					break;
			}

			return TRUE;
		}
	}*/

	return FALSE;
}

//Tracker 개수 얻기
int CStaticMatDisplay::GetNum()
{
	//return m_Tracker.GetCount();
	return 0;
}

//Tracker 종류 얻기
int CStaticMatDisplay::GetType(int idx)
{
	/*C_CtrlTracker tracker;

	if(!m_Tracker.IsEmpty())
	{
		POSITION pos = m_Tracker.FindIndex(idx);
		if(NULL != pos)
		{
			tracker = m_Tracker.GetAt(pos);
			return tracker.GetType();
		}
	}*/
	
	return -1;
}

//현재 트랙커 화면 활성화 여부
void CStaticMatDisplay::SetActiveImageRect(BOOL active)
{
	m_bActive = active;
}

void CStaticMatDisplay::SetActiveImageRect(int idx, BOOL active)
{
	/*C_CtrlTracker tracker;

	if(!m_Tracker.IsEmpty())
	{
		POSITION pos = m_Tracker.FindIndex(idx);
		if(NULL != pos)
		{
			tracker = m_Tracker.GetAt(pos);
			tracker.SetActive(active);
		}
	}*/
}

void CStaticMatDisplay::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	


	CStatic::OnLButtonDblClk(nFlags, point);
}
