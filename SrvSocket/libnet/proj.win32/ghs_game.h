#pragma once
#include "Singleton.h"
class CmdPacket;
class GHS_Game
{
public:
	GHS_Game(void);
	virtual ~GHS_Game(void);
public:
	void addPlayer(UI32 index,UI64 serial);
	void removePlayer(UI32 index,UI64 serial);
	void onRead(CmdPacket* packet);
	void send(UI32 id,CmdPacket *packet);
};

typedef Singleton<GHS_Game> SGHS_Game;