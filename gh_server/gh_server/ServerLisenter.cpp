#include "ServerLisenter.h"

void ServerLisenter::Process()
{
	ServerSession* session=AcceptNewSession();
	while(session)
	{
		m_sessions.push_back(session);
		session=AcceptNewSession();
	}
	vector<ServerSession*>::iterator itr;
	for(itr=m_sessions.begin();itr!=m_sessions.end();)
	{
		ServerSession* session=*itr;
		session->Process();
		if(session->GetState()==NET_STATE_ZERO)
		{
			CloseSession(session);
			itr=m_sessions.erase(itr);
		}
		else
		{
			++itr;
		}
	}
}

ServerLisenter::ServerLisenter()
{

}

ServerLisenter::~ServerLisenter()
{
	vector<ServerSession*>::iterator itr;
	for(itr=m_sessions.begin();itr!=m_sessions.end();++itr)
		delete *itr;
	m_sessions.clear();
}

ServerSession* ServerLisenter::AcceptNewSession()
{
	if(!m_socket.CanRead()) return false;
	SOCKET socket = m_socket.Accept();
	if(!socket) return false;

	
	ServerSession *tmp = new ServerSession;
	tmp->Attach(socket);
	/* set encrypt seed */
	tmp->SendSeed(ComputeSeed());
	
	OnAccept(tmp);
	return tmp;
}

void ServerLisenter::CloseSession( ServerSession* session )
{

	delete session;
}

void ServerLisenter::OnAccept( ServerSession* sessioon )
{
	char addr[128];
	short port;
	sessioon->GetSocket()->GetRemoteAddr(addr,&port);
	Sys_Log("Accept %s:%d",addr,port);
}

