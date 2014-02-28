#pragma once
#include "cmdpacket.h"
class GHS_Game;
class GHS_Player
{
public:
	GHS_Player(UI32 index,UI64 serial);
	virtual ~GHS_Player(void);
	inline UI64 getSerial()
	{
		return m_serial;
	}
private:
	UI32 m_index;
	UI64 m_serial;
	CmdPacket m_packet;
public:
	void sendGameInit();
	void sendGameStart(bool bottom);
	void sendGameShot(float x,float y);
	bool m_ready;
	GHS_Player* m_other;
	GHS_Game* m_game;
};

