/************************************************************************/
/*
@author:  junliang
@brief:   iocp io, ���߳�
*/
/************************************************************************/
#ifndef _WIN_IOCP_IO_SERVICE_H_
#define _WIN_IOCP_IO_SERVICE_H_

#include "socket_wrap.h"
//#include <mswsock.h>
#include "yy_freelist.h"
#include "sockbuf.h"
#include "peer_manager.h"
#include "io_service.h"
NS_YY_BEGIN


enum IO_OPERATION
{
	ClientIoUnknown,
	ClientIoRead,
	ClientIoWrite,
};

//ÿ��wsasend, wsarecv��Ӧ���ύһ���µ�overlap,��ű��β���
//Ƶ�������ͷ��ڴ棬��freelist
struct IOContext
{
	OVERLAPPED overlapped;		//һ��Ҫ�ڵ�һλ��ǿת��
	IO_OPERATION io_operation;
};


class Win_IOCP_IOService
{
public:
	Win_IOCP_IOService(OnConCallback onCon, OnDisConCallback onDis, OnReadCallback onRead);
	~Win_IOCP_IOService();

	void eventLoop(UI32 count=1024, UI32 time_out=1000);

	bool connectPeer(const char* ip, UI32 port);

	void listen(const char* ip, UI32 port);

	void send(UI32 index, UI64 serial, const char* buf, UI32 len);

	void closePeer(UI32 index, UI64 serial);
private:
	bool onEventTick(UI32 time_out);

	//����Ƿ���������
	void onAcceptTick();

	//��ָ��socket�󶨵���ɶ˿���
	void updateCompletionPort(SOCKET socket, UI32 index);

	//Ͷ������
	void postRecvRequest(UI32 index, UI64 serial);
	void postSendRequest(UI32 index, UI64 serial);

	//������������ӻ��߿ͻ������ӳɹ���ע���socket����ɶ���
	void registerSocket(SOCKET sockfd);

	//��������io context
	IOContext* newIOCxt(IO_OPERATION type);
	void deleteIOCxt(IOContext* io_cxt);

	void processWrite(UI32 index, UI32 size);
	void processRead(UI32 index, UI32 size);
private:
	//accept event
	SOCKET m_listen_sd;
	fd_set m_read_set;		//������

	HANDLE m_iocp_handle;
	FreeList<IOContext> m_iocxt_pool;		//ÿ�η��ͽ�����Ϣ��ҪͶ��io context,���Ըýṹ����freelist������ڴ�ʹ����

	//�����¼�
	OnConCallback m_onConCallback;
	OnDisConCallback m_onDisConCallback;
	OnReadCallback m_onReadCallback;

	PeerManager m_peer_manager;			//�������ӽڵ�Ĺ�������+serial��־λ����֤Ψһ��(��new, delete�Ļ������ܻ��յ���ζϿ�֪ͨ�������ظ�ɾ�����ظ�֪ͨ��)
};

NS_YY_END
#endif