#pragma once
class GHS_Player;
class GHS_Game
{
public:
	GHS_Game(GHS_Player*,GHS_Player*);
	~GHS_Game(void);
public:
	GHS_Player* m_player1;
	GHS_Player* m_player2;
};

