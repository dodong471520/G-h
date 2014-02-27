#include "stdafx.h"
#include "GHS_Player.h"
#include "GHS_Lobby.h"

GHS_Player::GHS_Player( UI32 index,UI64 serial )
	:m_index(index),m_serial(serial)
{

}


GHS_Player::~GHS_Player(void)
{
}

void GHS_Player::sendReady()
{
	m_packet.BeginWrite();
	m_packet.WriteUShort(PROTO_Game_Init);
	SGHS_Game::shared()->send(m_index,m_serial,&m_packet);
}
