#pragma once
//#include "_CtrlTracker.h"

// CStaticMatDisplay

class CStaticMatDisplay : public CStatic
{
	DECLARE_DYNAMIC(CStaticMatDisplay)

public:
	CStaticMatDisplay(BOOL bUseTracker = FALSE, BOOL bWheelZoom = FALSE, BOOL bImageDrag = FALSE);
	virtual ~CStaticMatDisplay();

private:
	CBitmap*	m_pBitmap			;	//	더블 버퍼링
	POINT		m_ptImagePos		;	//	윈도우 중앙 기준 이미지 중앙 좌표
	POINT		m_ptLBDown			;	//	마우스 좌측 버튼 누른 시점의 기준 좌표
	POINT		m_ptBeforImagePos	;	//	마우스 좌측 버튼 누른 시점의 m_ptImagePos
	float		m_fScale			;	//	이미지 확대/축소 배율
	int			m_iScale			;	//	이미지 확대/축소 상수(+일 경우 곱하기, -일 경우 나누기)
	BOOL		m_bImageMoving		;	//	마우스 좌측 버튼 누른 후 땔때까지 TRUE, 그 외에는 FALSE
	BOOL		m_bWheelZoom		;	//	마우스 휠 zoom 사용
	BOOL		m_bImageDrag		;
	BOOL		m_bUseTracker		;
	int m_nMouseCurX;
	int m_nMouseCurY;
	BYTE m_byMouseCurVal;

	BOOL m_bActive;
	int m_iActiveIdx;
	//현재 색
	COLORREF m_curColor;
	long m_lZoomStartX;
	long m_lZoomStartY;
	long m_lZoomEndX;
	long m_lZoomEndY;

	VOID	CalcAndDisplayImage()	;

public:
	//BOOL	SetImage( IplImage* pImage )	;
	BOOL SetImage(Mat img);
	VOID	ZoomIn()						;
	VOID	ZoomOut()						;
	VOID	Fit()							;
	VOID	OriginalScale()					;

	BYTE GetCurImageVal(){return m_byMouseCurVal;};
	CPoint GetCurImagePoint(){return CPoint(m_nMouseCurX, m_nMouseCurY);};

	CRect m_WinRect;
	CRect m_WinRectOld;

	void SetUseTracker(BOOL isTracker){m_bUseTracker = isTracker;};
	
	void Draw();
	//사각형 영역 추가
	void AddTracker(int type, CRect rect, COLORREF color, char *cap);
	//모든 영역 삭제
	void DeleteAll();
	//영역 삭제(생성해 있지 않으면 FALSE 반환)
	BOOL DeleteTracker(int idx);
	//사각형 영역 얻기(생성해 있지 않으면 FALSE 반환)
	BOOL GetTracker(int idx, CRect *rect);
	//사각형 영역 설정(생성해 있지 않으면 FALSE 반환)
	BOOL SetTracker(int idx, CRect rect);
	BOOL SetGroup(int idx, BOOL grp);
	//Tracker 개수 얻기
	int GetNum();
	//Tracker 종류 얻기
	int GetType(int idx);
	//현재 트랙커 화면 활성화 여부
	void SetActiveImageRect(BOOL active);
	void SetActiveImageRect(int idx, BOOL active);

	void UpdateDisplay(){CalcAndDisplayImage();};
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


