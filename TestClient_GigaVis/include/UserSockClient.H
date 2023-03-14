//clientsok.h
//클라이언트용 소켓 프로그램
#define WM_RECEIVE_DATA			WM_USER+2
#define WM_ONCONNECT			WM_USER+3
#define WM_ONCLOSE				WM_USER+4
#define AUTO_CONNECT_DELAY		100

#define PACKET_MAX_NUM				256
#define PACKET_CHAR_STX					0x02
#define PACKET_CHAR_ETX					0x03

class CClientSock : public CAsyncSocket
{

public:
		CClientSock();
		void SetWnd(HWND hwnd);

		virtual void OnReceive( int nErrorCode );
		virtual void OnConnect( int nErrorCode);
		virtual void OnClose(int nErrorCode);
public:
		char m_pData[200];
		HWND m_pHwnd;
};