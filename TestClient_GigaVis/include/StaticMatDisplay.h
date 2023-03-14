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
	CBitmap*	m_pBitmap			;	//	���� ���۸�
	POINT		m_ptImagePos		;	//	������ �߾� ���� �̹��� �߾� ��ǥ
	POINT		m_ptLBDown			;	//	���콺 ���� ��ư ���� ������ ���� ��ǥ
	POINT		m_ptBeforImagePos	;	//	���콺 ���� ��ư ���� ������ m_ptImagePos
	float		m_fScale			;	//	�̹��� Ȯ��/��� ����
	int			m_iScale			;	//	�̹��� Ȯ��/��� ���(+�� ��� ���ϱ�, -�� ��� ������)
	BOOL		m_bImageMoving		;	//	���콺 ���� ��ư ���� �� �������� TRUE, �� �ܿ��� FALSE
	BOOL		m_bWheelZoom		;	//	���콺 �� zoom ���
	BOOL		m_bImageDrag		;
	BOOL		m_bUseTracker		;
	int m_nMouseCurX;
	int m_nMouseCurY;
	BYTE m_byMouseCurVal;

	BOOL m_bActive;
	int m_iActiveIdx;
	//���� ��
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
	//�簢�� ���� �߰�
	void AddTracker(int type, CRect rect, COLORREF color, char *cap);
	//��� ���� ����
	void DeleteAll();
	//���� ����(������ ���� ������ FALSE ��ȯ)
	BOOL DeleteTracker(int idx);
	//�簢�� ���� ���(������ ���� ������ FALSE ��ȯ)
	BOOL GetTracker(int idx, CRect *rect);
	//�簢�� ���� ����(������ ���� ������ FALSE ��ȯ)
	BOOL SetTracker(int idx, CRect rect);
	BOOL SetGroup(int idx, BOOL grp);
	//Tracker ���� ���
	int GetNum();
	//Tracker ���� ���
	int GetType(int idx);
	//���� Ʈ��Ŀ ȭ�� Ȱ��ȭ ����
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


