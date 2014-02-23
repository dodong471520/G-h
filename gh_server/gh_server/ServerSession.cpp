#include "ServerSession.h"


void ServerSession::OnConnect( bool ret )
{
	char addr[128];
	short port;
	m_socket.GetRemoteAddr(addr,&port);
	Sys_Log("Connected %s:%d",addr,port);
}

ServerSession::ServerSession()
{

}

ServerSession::~ServerSession()
{

}

void ServerSession::OnDisconnect()
{
	char addr[128];
	short port;
	m_socket.GetRemoteAddr(addr,&port);
	Sys_Log("Disconnectd %s:%d",addr,port);
}

void ServerSession::OnReceive( CG_CmdPacket *packet )
{
	char addr[128];
	short port;
	m_socket.GetRemoteAddr(addr,&port);
	short cmd=0;
	packet->ReadShort(&cmd);
	char * buff=0;
	packet->ReadString(&buff);
	Sys_Log("%s:%d==%d=>%s",addr,port,cmd,buff);
}
