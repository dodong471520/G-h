// gh_server.cpp : �������̨Ӧ�ó������ڵ㡣
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

