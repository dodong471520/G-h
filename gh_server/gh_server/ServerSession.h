#pragma once
#include "gamelib/g_tcpsession.h"

class ServerSession:public CG_TCPSession
{
public:
	ServerSession();
	virtual ~ServerSession();
	virtual void OnConnect(bool ret);
	virtual void OnDisconnect();
	virtual void OnReceive(CG_CmdPacket *packet);
	CG_NetSocket* GetSocket()
	{
		return &m_socket;
	}
};