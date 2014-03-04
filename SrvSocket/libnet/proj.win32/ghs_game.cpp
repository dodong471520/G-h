#include "stdafx.h"
#include "GHS_Game.h"
#include "GHS_Player.h"

GHS_Game::GHS_Game( GHS_Player* player1,GHS_Player*player2 )
	:m_player1(player1),m_player2(player2)
{
}


GHS_Game::~GHS_Game(void)
{
}

void GHS_Game::process()
{

}

void GHS_Game::init()
{
	m_player1->m_game=this;m_player1->m_other=m_player2;
	m_player2->m_game=this;m_player2->m_other=m_player1;
	m_player1->m_bSer=true;
	m_player2->m_bSer=false;
	/*if(m_player1->m_rtt<=m_player2->m_rtt)
	{
	m_player1->m_bSer=true;
	m_player2->m_bSer=false;
	}
	else
	{
	m_player1->m_bSer=false;
	m_player2->m_bSer=true;
	}*/
	m_player1->m_bottom=true;
	m_player2->m_bottom=false;
	m_player1->sendGameInit();
	m_player2->sendGameInit();
}

void GHS_Game::start()
{
	m_player1->sendGameStart();
	m_player2->sendGameStart();
}
