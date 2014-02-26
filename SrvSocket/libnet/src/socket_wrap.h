#ifndef _SOCKET_WRAP_H_
#define _SOCKET_WRAP_H_

#include "yy_macros.h"
#include <winsock2.h>
#include "yy_exception.h"
NS_YY_BEGIN

//������io
inline void NetInit();
inline void NetDestroy();
inline int CreateSocket();
inline void CloseSocket(SOCKET sd);

//��������socket��Ĭ������Ϊ������
inline void ListenSocket(SOCKET &listen_sd, const char* ip, UI32 port);


//�������ӣ���socketΪ������, ����ֵ�ж��Ƿ����ӳɹ�
inline SOCKET ConnectSocket(SOCKET listen_sd, const char* ip, int port);

//������accept, ����ֵ�ж��Ƿ�accept�ɹ�
inline bool AcceptSocket(SOCKET listen_sd, SOCKET& new_sd);



//////////////////////////////////////////////////////////////////////////////
void NetInit()
{
	WSADATA wsaData;
	if((WSAStartup(MAKEWORD(2,2), &wsaData)) != 0)
	{
		THROWNEXCEPT("WSAStartup() failed\n");
	}
}

void NetDestroy()
{
	WSACleanup();
}

int CreateSocket()
{
	//create socket
	SOCKET new_socket = ::socket(	AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(new_socket == INVALID_SOCKET)
	{
		THROWNEXCEPT("create socket error.errno:%d", WSAGetLastError());
	}

	return new_socket;
}

void CloseSocket(SOCKET sd)
{
	::closesocket(sd);
}

void ListenSocket(SOCKET &listen_sd, const char* ip, UI32 port)
{
	if(NULL == ip)
		THROWNEXCEPT("ip is null.");

	listen_sd = CreateSocket();

	sockaddr_in service;
	service.sin_family = AF_INET;

	//���ַ���ת����32λ�������ֽ���
	service.sin_addr.s_addr = inet_addr(ip);

	//��16λ�˿ںŴ������ֽ���ת���������ֽ���
	service.sin_port = htons(port);

	if(::bind( listen_sd, (SOCKADDR*) &service, sizeof(service)) == SOCKET_ERROR)
		THROWNEXCEPT("bind socket error.errno:%d", WSAGetLastError());

	u_long iMode = 1;  //non-blocking mode is enabled.
	ioctlsocket(listen_sd, FIONBIO, &iMode); //����Ϊ������ģʽ

	if( ::listen(listen_sd, 5) == SOCKET_ERROR )
		THROWNEXCEPT("listen socket erro.errno:%", WSAGetLastError());

	
	//�رշ��ͻ���,������listen֮ǰ����,����wsasend��ʱ��wsabufָ���û����������ڴ�������
	//int nZero=0;
	//assert(setsockopt(m_listen_sd, SOL_SOCKET, SO_SNDBUF, (char *)&nZero, sizeof(nZero)) != SOCKET_ERROR);
	//assert( ::listen(m_listen_sd, 5) != SOCKET_ERROR );

	/*
	//�ر����ӵ�ʱ�򣬷��ͻ������ͽ��ܻ������е��������ݶ�����, time_wait�����Ե��
	LINGER lingerStruct;
	lingerStruct.l_onoff = 1;
	lingerStruct.l_linger = 0;

	nRet = setsockopt(m_listen_sd, SOL_SOCKET, SO_LINGER,
	(char *)&lingerStruct, sizeof(lingerStruct) );
	if( nRet == SOCKET_ERROR )
	{
		THROWNEXCEPT("setsockopt(SO_LINGER) failed: %d\n", WSAGetLastError());
	}*/
}

//����ͻ��˺ͷ������ͬһ�������������connectͨ�������������ӣ�
//���򷵻�EINPROGRESS, ��ʾ���ӽ����Ѿ�����������δ��ɣ��Ѿ������tcp��·���ּ�������
//������connect��ֲ�Բ��ã�����һ�㶼�ǽ��̸�������ʱ�����Ĳ�����������������ʽ�����ܻ�������ʮ�룬Ҳ�ǿ��Ե�
SOCKET ConnectSocket(SOCKET listen_sd, const char* ip, int port)
{
	SOCKET new_sd=CreateSocket();

	//connect to server
	sockaddr_in server_addr;
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port=htons(port);
	if(::connect(new_sd, (sockaddr*)&server_addr, sizeof(server_addr)) ==-1 )
	{
		CloseSocket(new_sd);
		return INVALID_SOCKET;
	}

	//����Ϊ������ģʽ
	u_long iMode = 1;  //non-blocking mode is enabled.
	if(ioctlsocket(new_sd, FIONBIO, &iMode) == SOCKET_ERROR)
		THROWNEXCEPT("set no block mode erro. errno:%d", WSAGetLastError());

	return new_sd;
}

//��ʹ����select�жϣ�Ҳ�п�����select�ɹ��󣬿ͻ��˶Ͽ����ӣ�����accept����������Ҫ�÷�����io
bool AcceptSocket(SOCKET listen_sd, SOCKET& new_sd)
{
	sockaddr_in clientAddr;
	int nClientLen = sizeof(sockaddr_in);

	//������accept
	new_sd=::accept(listen_sd, (sockaddr*)&clientAddr, &nClientLen);
	if(new_sd == INVALID_SOCKET)
	{
		int err_num=WSAGetLastError();
		if(err_num==WSAEWOULDBLOCK)
			return false;

		THROWNEXCEPT("accept socket erro. errno:%d", err_num);
	}

	//�������ӽ��������óɷ�������
	u_long iMode = 1;  //non-blocking mode is enabled.
	if(ioctlsocket(new_sd, FIONBIO, &iMode) == SOCKET_ERROR)
		THROWNEXCEPT("set no block mode erro. errno:%d", WSAGetLastError());

	return true;
}

NS_YY_END
#endif