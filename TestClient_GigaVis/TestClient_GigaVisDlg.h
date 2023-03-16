
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

	int m_nDisplayIdx;
	BYTE* m_byRcvFullBuff;//Recive Full Data;
	int m_nRcvFullBuffIdx;//First Recive Data Len
	int m_nRcvFullBuffLen;//Recive Full Data Len
	BOOL m_bRcvFullBuff;//Recive Full Data Flag
	int m_nRcvImgSize;//Recive Image Data Size
	int m_nImageDataIdx;//Recive Data Start Image Data Index
private:



public:


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
