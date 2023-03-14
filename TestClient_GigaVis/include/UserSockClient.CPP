#include "pch.h"
#include "UserSockClient.h"



CClientSock::CClientSock()
{
	CAsyncSocket::CAsyncSocket();
}

void CClientSock::SetWnd(HWND hwnd)
{
	m_pHwnd=hwnd;
}


void CClientSock::OnReceive( int nErrorCode )
{
	 //TRACE("Errocode = %d",nErrorCode);
	 SendMessage(m_pHwnd,WM_RECEIVE_DATA,0,0);
}

void CClientSock::OnConnect( int nErrorCode )
{
	//TRACE("Errocode = %d",nErrorCode);
	SendMessage(m_pHwnd,WM_ONCONNECT, nErrorCode, 0);
}

void CClientSock::OnClose( int nErrorCode)
{
	//TRACE("Errocode = %d",nErrorCode);
	SendMessage(m_pHwnd,WM_ONCLOSE, 0, 0);
}