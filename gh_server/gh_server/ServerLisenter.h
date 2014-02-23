#pragma once

#include "gamelib/g_tcplistener.h"
#include "ServerSession.h"

class ServerLisenter:public CG_TCPListener
{
public:
	ServerLisenter();
	virtual ~ServerLisenter();
	ServerSession* AcceptNewSession();
	void Process();
	void CloseSession(ServerSession* session);
	virtual void OnAccept(ServerSession* sessioon);
public:
	vector<ServerSession*> m_sessions;
};