#include "stdafx.h"
#include "GHS_Player.h"
#include "GHS_Lobby.h"
#include "GHS_Game.h"

GHS_Player::GHS_Player( UI32 index,UI64 serial )
	:m_index(index),m_serial(serial),m_ready(false),m_game(0),m_other(0)
	,m_lstSendTime(0),m_lstRecvTime(0),m_bSer(false)
{
	m_lstRecvTime=GetTickCount();
	m_lstSendTime=GetTickCount();
}


GHS_Player::~GHS_Player(void)
{
}

void GHS_Player::sendGameInit()
{
	m_packet.BeginWrite();
	m_packet.WriteUShort(PROTO_S_GameInit);
	send(&m_packet);
}

void GHS_Player::sendGameStart()
{
	m_packet.BeginWrite();
	m_packet.WriteUShort(PROTO_S_GameStart);
	m_packet.WriteByte(m_bSer!=0);
	m_packet.WriteByte(m_bottom);
	send(&m_packet);
}

void GHS_Player::sendGameShot(float x,float y)
{
	m_packet.BeginWrite();
	m_packet.WriteUShort(PROTO_S_GameShot);
	m_packet.WriteFloat(x);
	m_packet.WriteFloat(y);
	send(&m_packet);
}

void GHS_Player::keepAlive()
{
	m_packet.BeginWrite();
	m_packet.WriteUShort(PROTO_Keep_Alive);
	m_packet.WriteUInt(GetTickCount());
	send(&m_packet);
}

void GHS_Player::send(CmdPacket *packet)
{
	SGHS_Game::shared()->send(m_index,m_serial,packet);
	m_lstSendTime=GetTickCount();
}
void GHS_Player::process()
{
	UI32 now=GetTickCount();
	if(now-m_lstSendTime>10*1000)
	{
		keepAlive();
	}
	if(now-m_lstRecvTime>30*1000)
	{
		//dead
	}
}

void GHS_Player::recv( CmdPacket* packet )
{
	m_lstRecvTime=GetTickCount();
	UI16 cmd;
	packet->ReadUShort(&cmd);
	switch(cmd)
	{
	case PROTO_Keep_Alive:
		{
			UINT32 time;
			if(packet->ReadUInt(&time))
			{
				m_packet.BeginWrite();
				m_packet.WriteUShort(PROTO_Keep_Alive_Ack);
				m_packet.WriteUInt(time);
				send(&m_packet);
			}
			break;
		}
	case PROTO_Keep_Alive_Ack:
		{
			UINT32 time;
			if(packet->ReadUInt(&time))
				m_rtt=GetTickCount()-time;
			break;
		}
	case PROTO_Synch_Pos:
		{
			if(m_bSer&&m_other)
			{
				CmdPacket packet2;
				packet2.BeginWrite(packet->GetReadData(),packet->GetDataSize());
				m_other->send(&packet2);
			}
			break;
		}
	case PROTO_C_AutoMatch:
		{
			SGHS_Game::shared()->enqueue(this);
			break;
		}
	case PROTO_C_UAutoMatch:
		{
			SGHS_Game::shared()->unqueue(this);
			break;
		}
	case PROTO_C_GameReady:
		{
			m_ready=true;
			if(m_other&&m_other->m_ready)
			{
				m_game->start();
			}
			break;
		}
	case PROTO_C_GameShot:
		{
			if(m_game&&!m_bSer)
			{
				float x=0;
				packet->ReadFloat(&x);
				float y=0;
				packet->ReadFloat(&y);
				m_other->sendGameShot(x,y);
			}
		}
	}
}
