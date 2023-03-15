
// TestClient_GigaVisDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "TestClient_GigaVis.h"
#include "TestClient_GigaVisDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTestClientGigaVisDlg 대화 상자



CTestClientGigaVisDlg::CTestClientGigaVisDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TESTCLIENT_GIGAVIS_DIALOG, pParent)
{
	m_byRcvFullBuff = NULL;
	m_nRcvFullBuffIdx = 0;;
	m_nRcvFullBuffLen = 0;;
	m_bRcvFullBuff = FALSE;;
	m_nRcvImgSize = 0;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestClientGigaVisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_DISP, m_Display);
}

BEGIN_MESSAGE_MAP(CTestClientGigaVisDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_MESSAGE(WM_RECEIVE_DATA, OnReceive)
	ON_MESSAGE(WM_ONCONNECT, OnConnect)
	ON_MESSAGE(WM_ONCLOSE, OnClose)
END_MESSAGE_MAP()


// CTestClientGigaVisDlg 메시지 처리기

BOOL CTestClientGigaVisDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	if (!AfxSocketInit())
	{
		AfxMessageBox(_T("AfxSocketInit Error"));
	}
	else
	{
		m_Client = new CClientSock;
		m_Client->Create();
		m_Client->SetWnd(this->m_hWnd);
	}

	m_nTestIdx = 0;
	m_Image.create(1544, 2064, CV_8UC3);
	m_Display.SetImage(m_Image);
	m_Display.Fit();
	SetTimer(100, 100, NULL);
	//SetTimer(101, 1000, NULL);
	m_nDisplayIdx = 0;

	m_bThreadEnd = FALSE;
	m_pDisplayThread = AfxBeginThread(ThreadDisplay, this, THREAD_PRIORITY_NORMAL);
	m_pDisplayThread->m_bAutoDelete = TRUE;

	m_byRcvFullBuff = new BYTE[10000000];

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CTestClientGigaVisDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CTestClientGigaVisDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CTestClientGigaVisDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

UINT CTestClientGigaVisDlg::ThreadDisplay(LPVOID pParam)
{
	CTestClientGigaVisDlg* pdlg = (CTestClientGigaVisDlg*)pParam;
	pdlg->DisplayThread();

	return 0;
}

void CTestClientGigaVisDlg::DisplayThread()
{
	while (TRUE)
	{
		if (m_RcvImage.size() > 0)
		{
			
			if (m_nDisplayIdx < m_RcvImage.size() && m_RcvImage[m_nDisplayIdx++].cols != 0 && m_RcvImage[m_nDisplayIdx++].rows != 0)
				m_Display.SetImage(m_RcvImage[m_nDisplayIdx]);

			Sleep(1000);
			//m_RcvImage.pop();

			if (m_nDisplayIdx > m_RcvImage.size())
				m_nDisplayIdx = 0;

		}


		if (m_bThreadEnd)
			break;

		Sleep(1);
	}
}

void CTestClientGigaVisDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CStringA strText;
	switch ((int)nIDEvent)
	{
	case 100:
		if (!m_bClientConnect)
		{
			m_Client->Connect(_T("127.0.0.1"), 5000);
		}
		break;
	case 101:
		if (m_RcvImage.size() > 0)
		{
			m_Display.SetImage(m_RcvImage.front());
			Sleep(1000);
			//m_RcvImage.pop();
		}
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

LRESULT CTestClientGigaVisDlg::OnReceive(WPARAM wParam, LPARAM lParam)
{
	CString strText, strTemp, strRslt;
	BYTE* byData;
	int nMaxLen = 10000000;
	byData = new BYTE[10000000];

	int nRcvLen = m_Client->Receive(byData, 10000000);

	strTemp.Format(_T("%S"), byData);
	if (byData[nRcvLen - 1] == PACKET_CHAR_ETX)
	{
		m_bRcvFullBuff = TRUE;
	}
	
	if (byData[0] == PACKET_CHAR_STX)
	{
		strTemp.Format(_T("%S"), byData);
		strText.Format(_T("%s"), strTemp.Mid(1, strTemp.GetLength()));

		AfxExtractSubString(strRslt, strText, 0, ',');
		int nHeight = _wtoi(strRslt);
		AfxExtractSubString(strRslt, strText, 1, ',');
		int nWidth = _wtoi(strRslt);
		AfxExtractSubString(strRslt, strText, 2, ',');
		int nBpp = _wtoi(strRslt);
		AfxExtractSubString(strRslt, strText, 3, ',');
		m_nRcvFullBuffLen = _wtoi(strRslt);

		//m_nImageDataIdx = 

		int nIdx = 0, nTempIdx = 0;
		for (int i = 0; i < 4; i++)
		{
			nIdx = strText.Find(',', nIdx+1);
		}

		m_nImageDataIdx = nIdx + 2;
		m_nRcvImgSize = nHeight * nWidth * nBpp;
		if (m_Image.rows != nHeight || m_Image.cols != nWidth || m_Image.channels() != nBpp)
		{
			if (m_Image.rows != 0 || m_Image.cols != 0)
				m_Image.release();

			m_Image.create(nHeight, nWidth, CV_8UC3);
		}
		
		memset(m_byRcvFullBuff, NULL, 10000000);
		if (m_nRcvFullBuffLen != nRcvLen)
		{
			m_nRcvFullBuffIdx = nRcvLen;
			m_bRcvFullBuff = TRUE;
			memcpy(m_byRcvFullBuff, byData, sizeof(BYTE) * m_nRcvFullBuffIdx);
		}
		else
		{
			m_bRcvFullBuff = FALSE;
			memcpy(&m_byRcvFullBuff[m_nRcvFullBuffIdx], byData, sizeof(BYTE) * nRcvLen);
			m_nRcvFullBuffIdx = 0;
			BYTE byRcvData[3];
			memcpy(m_Image.data, &byData[m_nImageDataIdx], m_nRcvImgSize);
			byRcvData[0] = byData[m_nImageDataIdx];
			byRcvData[1] = byData[m_nImageDataIdx + 1];
			byRcvData[2] = byData[m_nImageDataIdx + 2];

			//m_RcvImage.push_back(m_Image);
			m_Display.Fit();
			m_Display.SetImage(m_Image);
			m_Display.UpdateDisplay();
			delete[] byData;

			byData = new BYTE[512];

			int nIdx = 0;
			byData[nIdx++] = PACKET_CHAR_STX;
			byData[nIdx++] = 'R';
			byData[nIdx++] = 'C';
			byData[nIdx++] = 'V';
			byData[nIdx++] = ',';
			byData[nIdx++] = byRcvData[0];
			byData[nIdx++] = byRcvData[1];
			byData[nIdx++] = byRcvData[2];
			byData[nIdx++] = PACKET_CHAR_ETX;

			m_Client->Send(byData, nIdx);

		}
		
	}
	else
	{
		if (m_nRcvFullBuffLen == (m_nRcvFullBuffIdx + nRcvLen))
		{
			m_bRcvFullBuff = FALSE;
			memcpy(&m_byRcvFullBuff[m_nRcvFullBuffIdx], byData, sizeof(BYTE) * nRcvLen);
			m_nRcvFullBuffIdx = 0;
			BYTE byRcvData[6];
			memcpy(m_Image.data, &m_byRcvFullBuff[m_nImageDataIdx], m_nRcvImgSize);
			byRcvData[0] = m_byRcvFullBuff[m_nImageDataIdx];
			byRcvData[1] = m_byRcvFullBuff[m_nImageDataIdx + 1];
			byRcvData[2] = m_byRcvFullBuff[m_nImageDataIdx + 2];
			byRcvData[3] = m_byRcvFullBuff[m_nImageDataIdx + 3];
			byRcvData[4] = m_byRcvFullBuff[m_nImageDataIdx + 4];
			byRcvData[5] = m_byRcvFullBuff[m_nImageDataIdx + 5];

			//m_RcvImage.push_back(m_Image);
			m_Display.Fit();
			m_Display.SetImage(m_Image);
			m_Display.UpdateDisplay();
			delete[] byData;


			byData = new BYTE[512];

			int nIdx = 0;
			byData[nIdx++] = PACKET_CHAR_STX;
			byData[nIdx++] = 'R';
			byData[nIdx++] = 'C';
			byData[nIdx++] = 'V';
			byData[nIdx++] = ',';
			byData[nIdx++] = byRcvData[0];
			byData[nIdx++] = byRcvData[1];
			byData[nIdx++] = byRcvData[2];
			byData[nIdx++] = byRcvData[3];
			byData[nIdx++] = byRcvData[4];
			byData[nIdx++] = byRcvData[5];
			byData[nIdx++] = PACKET_CHAR_ETX;

			m_Client->Send(byData, nIdx);

			
		}
		else
		{

		}
	}

	delete[] byData;
	return 0;
}

LRESULT CTestClientGigaVisDlg::OnConnect(WPARAM wParam, LPARAM lParam)
{
	int err = (int)wParam;
	CString strCap;

	switch (err)
	{
	case NULL:				//연결 성공
		KillTimer(100);
		m_bClientConnect = TRUE;
		break;
	}

	return 0;
}

LRESULT CTestClientGigaVisDlg::OnClose(WPARAM wParam, LPARAM lParam)
{
	CString strCap;

	m_bClientConnect = FALSE;

	delete m_Client;
	m_Client = NULL;

	m_Client = new CClientSock();
	m_Client->Create();
	m_Client->SetWnd(this->m_hWnd);
	SetTimer(100, AUTO_CONNECT_DELAY, NULL);

	return 0;
}


BOOL CTestClientGigaVisDlg::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_bThreadEnd = TRUE;
	delete[] m_byRcvFullBuff;

	return CDialogEx::DestroyWindow();
}
