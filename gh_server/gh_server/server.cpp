// gh_server.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "ServerLisenter.h"

ServerLisenter g_listener;
int _tmain(int argc, _TCHAR* argv[])
{
	g_listener.StartListen("127.0.0.1",8885);
	while(true)
	{
		g_listener.Process();
		Sleep(100);
	}
	return 0;
}

