#pragma once
#include "Singleton.h"
class CmdPacket;
#include "sockbuf.h"
class GHS_Player;
class GHS_Game
{
private:
	typedef FastDelegate4<UI32, UI64, const char*, UI32> OnSendCallback;
private:
	OnSendCallback onSend_;
public:
	inline void registerOnSend(OnSendCallback onSend)
	{
		onSend_=onSend;
	}
public:
	GHS_Game(void);
	virtual ~GHS_Game(void);
public:
	void addPlayer(UI32 index,UI64 serial);
	void removePlayer(UI64 serial );
	GHS_Player* getPlayer(UI64 serial);
	void onRead(UI32 index,UI64 serial,CmdPacket* packet);
	void send(UI32 index,UI64 serial,CmdPacket *packet);
private:
	SockBuf m_writeBuff;
	map<UI64,GHS_Player*> m_players;
	typedef map<UI64,GHS_Player*>::iterator MAP_PLAYER_ITOR;

	vector<GHS_Player*> m_queue;
	typedef vector<GHS_Player*>::iterator VEC_PLAYER_ITOR;
};

typedef Singleton<GHS_Game> SGHS_Game;