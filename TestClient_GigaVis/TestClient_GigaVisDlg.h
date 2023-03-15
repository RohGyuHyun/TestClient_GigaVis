
// TestClient_GigaVisDlg.h: 헤더 파일
//

#pragma once
#include "UserSockClient.h"
#include "StaticMatDisplay.h"
// CTestClientGigaVisDlg 대화 상자
class CTestClientGigaVisDlg : public CDialogEx
{
// 생성입니다.
public:
	CTestClientGigaVisDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TESTCLIENT_GIGAVIS_DIALOG };
#endif

protected:
	CClientSock* m_Client;
	BOOL m_bClientConnect;
	Mat m_Image;
	CStaticMatDisplay m_Display;
	vector<Mat> m_RcvImage;


	CWinThread* m_pDisplayThread;
	UINT static ThreadDisplay(LPVOID pParam);
	BOOL m_bThreadEnd;
	int m_nDisplayIdx;
	int m_nTestIdx;
	BYTE* m_byRcvFullBuff;
	int m_nRcvFullBuffIdx;
	int m_nRcvFullBuffLen;
	BOOL m_bRcvFullBuff;
	int m_nRcvImgSize;
	int m_nImageDataIdx;
private:



public:
	void DisplayThread();

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnReceive(WPARAM, LPARAM);
	afx_msg LRESULT OnConnect(WPARAM, LPARAM);
	afx_msg LRESULT OnClose(WPARAM wParam, LPARAM lParam);
	virtual BOOL DestroyWindow();
};
