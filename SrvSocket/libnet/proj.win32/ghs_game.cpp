#include "stdafx.h"

#include "ghs_game.h"
#include "ghs_player.h"
GHS_Game::GHS_Game(void)
{

}


GHS_Game::~GHS_Game(void)
{
	for(MAP_PLAYER_ITOR itor=m_players.begin();itor!=m_players.end();++itor)
	{
		if(itor->second)
			removePlayer(itor->second->getSerial());
	}
}

void GHS_Game::addPlayer( UI32 index,UI64 serial )
{
	assert(m_players.find(serial)==m_players.end());
	m_players.insert(make_pair(serial,new GHS_Player(index,serial)));
}

void GHS_Game::removePlayer(UI64 serial )
{
	MAP_PLAYER_ITOR itor=m_players.find(serial);
	if(itor!=m_players.end())
	{
		GHS_Player* player=itor->second;

		m_players.erase(itor);
	}
}

void GHS_Game::onRead(UI32 index,UI64 serial,CmdPacket* packet )
{
	GHS_Player* player=getPlayer(serial);
	if(player==NULL)
		return;
	UI16 cmd;
	packet->ReadUShort(&cmd);
	switch(cmd)
	{
	case PROTO_AutoMatch:
		{
			m_queue.push_back(player);
			
			break;
		}
	case PROTO_UAutoMatch:
		{
			VEC_PLAYER_ITOR itor=find(m_queue.begin(),m_queue.end(),player);
			if(itor!=m_queue.end())
				m_queue.erase(itor);
			break;
		}
	}
}

void GHS_Game::send( UI32 index,UI64 serial,CmdPacket *packet )
{
	if(index==-1)
		return;
	u_short packetLen=packet->GetDataSize();
	u_short packetLenN=htons(packetLen);
	m_writeBuff.reset();
	m_writeBuff.ensureWritableBytes(packetLen+2);
	char buff[2];
	memcpy(buff,&packetLenN,2);
	m_writeBuff.apendBuf(buff,2);
	m_writeBuff.apendBuf(packet->GetData(),packetLen);
	onSend_(index, serial,m_writeBuff.readStart(),m_writeBuff.readableBytes());
}

GHS_Player* GHS_Game::getPlayer( UI64 serial )
{
	return m_players[serial];
}
