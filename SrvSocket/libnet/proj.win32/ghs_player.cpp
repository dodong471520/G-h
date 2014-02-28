#include "stdafx.h"
#include "GHS_Player.h"
#include "GHS_Lobby.h"

GHS_Player::GHS_Player( UI32 index,UI64 serial )
	:m_index(index),m_serial(serial),m_ready(false),m_game(0),m_other(0)
{

}


GHS_Player::~GHS_Player(void)
{
}

void GHS_Player::sendGameInit()
{
	m_packet.BeginWrite();
	m_packet.WriteUShort(PROTO_S_GameInit);
	SGHS_Game::shared()->send(m_index,m_serial,&m_packet);
}

void GHS_Player::sendGameStart(bool bottom)
{
	m_packet.BeginWrite();
	m_packet.WriteUShort(PROTO_S_GameStart);
	m_packet.WriteByte(bottom);
	SGHS_Game::shared()->send(m_index,m_serial,&m_packet);
}

void GHS_Player::sendGameShot(float x,float y)
{
	m_packet.BeginWrite();
	m_packet.WriteUShort(PROTO_S_GameShot);
	m_packet.WriteFloat(x);
	m_packet.WriteFloat(y);
	SGHS_Game::shared()->send(m_index,m_serial,&m_packet);
}
