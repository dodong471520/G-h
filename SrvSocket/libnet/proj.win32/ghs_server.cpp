#include "stdafx.h"
#include <stdio.h>
#include <string>
#include "io_service.h"
#include "yy_thread.h"
#include "ghs_game.h"

USING_NS_YY
//#pragma comment(lib, "libbase.lib")

struct MsgHeader
{
	UI16 len;
};

#define MSGHEADERLEN sizeof(MsgHeader)

class NetEvent
{
public:
	NetEvent(){}

	//iocp thread or select io thread
	void onCon(UI32 index, UI64 serial, const char* ip, UI32 port)
	{
		SGHS_Game::shared()->addPlayer(index,serial);
		LOG_INFO("new connection, index:%d, serial:%llu, ip:%s, port:%d\n",index, serial, ip, port);
	}

	//io thread
	void onDisCon(UI32 index, UI64 serial, const char* ip, UI32 port)
	{
		SGHS_Game::shared()->removePlayer(serial);
		LOG_INFO("disconnection, index:%d, serial:%llu, ip:%s, port:%d\n",index, serial, ip, port);
	}

	//io thread
	int onRead(UI32 index, UI64 serial, SockBuf* buf)
	{
		const char* begin=buf->readStart();
		UI32 len=buf->readableBytes();
		//不完整的消息
		if(len <= MSGHEADERLEN)
			return 0;
		//根据网络协议，判断一条完整消息
		u_short packetLen=ntohs(*(u_short*)begin);
		//未满一条完整消息
		UI32 total_msg_len=MSGHEADERLEN+packetLen;
		if(len<total_msg_len)
			return 0;
		CmdPacket *packet=new CmdPacket();
		packet->BeginRead(begin+MSGHEADERLEN,packetLen);
		SGHS_Game::shared()->onRead(index,serial,packet);
		delete packet;
		buf->readMove(total_msg_len);
		return 0;
	}
};

int main()
{
	NetEvent net_event;
	IOService* io_service=new IOService(
		MakeDelegate(&net_event, &NetEvent::onCon),
		MakeDelegate(&net_event, &NetEvent::onDisCon),
		MakeDelegate(&net_event, &NetEvent::onRead));
	
	SGHS_Game::shared()->registerOnSend(MakeDelegate(io_service, &IOService::send));

	io_service->listen("127.0.0.1", 5001);
	LOG_INFO("server start 127.0.0.1:5001");
	while(true)
	{
		io_service->eventLoop(1024, 5);
		//OnFrame();
	}
	return 0;
}
