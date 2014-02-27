#include "stdafx.h"
#include "ghs_game.h"


GHS_Game::GHS_Game(void)
{

}


GHS_Game::~GHS_Game(void)
{
}

void GHS_Game::addPlayer( UI32 index,UI64 serial )
{

}

void GHS_Game::removePlayer( UI32 index,UI64 serial )
{

}

void GHS_Game::onRead( CmdPacket* packet )
{

}

void GHS_Game::send( UI32 id,CmdPacket *packet )
{
	char str[256];
	scanf("%s",str);

	if(m_index==-1)
		continue;
	u_short packetLen=strlen(str);
	char buff[256];
	u_short packetLenN=htons(packetLen);
	memcpy(buff,&packetLenN,MSGHEADERLEN);
	sprintf(buff+MSGHEADERLEN,str);
	onSend_(m_index, m_serial,buff,MSGHEADERLEN+packetLen);
}
