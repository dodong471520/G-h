#include "stdafx.h"

#include "GHS_Lobby.h"
#include "GHS_Player.h"
#include "GHS_Game.h"
GHS_Lobby::GHS_Lobby(void)
{

}


GHS_Lobby::~GHS_Lobby(void)
{
	for(MAP_PLAYER_ITOR itor=m_players.begin();itor!=m_players.end();++itor)
	{
		if(itor->second)
			removePlayer(itor->second->getSerial());
	}
}

void GHS_Lobby::addPlayer( UI32 index,UI64 serial )
{
	assert(m_players.find(serial)==m_players.end());
	m_players.insert(make_pair(serial,new GHS_Player(index,serial)));
}

void GHS_Lobby::removePlayer(UI64 serial )
{
	MAP_PLAYER_ITOR itor=m_players.find(serial);
	if(itor!=m_players.end())
	{
		GHS_Player* player=itor->second;

		m_players.erase(itor);
	}
}

void GHS_Lobby::onRead(UI32 index,UI64 serial,CmdPacket* packet )
{
	GHS_Player* player=getPlayer(serial);
	if(player==NULL)
		return;
	UI16 cmd;
	packet->ReadUShort(&cmd);
	switch(cmd)
	{
	case PROTO_C_AutoMatch:
		{
			m_queue.push_back(player);
			makeGame();
			break;
		}
	case PROTO_C_UAutoMatch:
		{
			VEC_PLAYER_ITOR itor=find(m_queue.begin(),m_queue.end(),player);
			if(itor!=m_queue.end())
				m_queue.erase(itor);
			break;
		}
	case PROTO_C_GameReady:
		{
			player->m_ready=true;
			if(player->m_other&&player->m_other->m_ready)
			{
				player->m_game->m_player1->sendGameStart(true);
				player->m_game->m_player2->sendGameStart(false);
			}
			break;
		}
	case PROTO_C_GameShot:
		{
			if(player->m_game)
			{
				float x=0;
				packet->ReadFloat(&x);
				float y=0;
				packet->ReadFloat(&y);
				player->m_other->sendGameShot(x,y);
			}
		}
	}
}

void GHS_Lobby::send( UI32 index,UI64 serial,CmdPacket *packet )
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

GHS_Player* GHS_Lobby::getPlayer( UI64 serial )
{
	return m_players[serial];
}

void GHS_Lobby::makeGame()
{
	if(m_queue.size()<2)
		return;
	VEC_PLAYER_ITOR itor=m_queue.begin();
	GHS_Player *player1=*itor;
	m_queue.erase(itor);
	itor=m_queue.begin();
	GHS_Player *player2=*itor;
	m_queue.erase(itor);
	GHS_Game *game=new GHS_Game(player1,player2);
	player1->m_game=game;player1->m_other=player2;
	player2->m_game=game;player2->m_other=player1;
	player1->sendGameInit();
	player2->sendGameInit();
	m_games.push_back(game);
}