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

void GHS_Game::start()
{
	m_player1->sendReady();
	m_player2->sendReady();
}
