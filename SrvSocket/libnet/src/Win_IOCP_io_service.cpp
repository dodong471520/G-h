#include "Win_IOCP_io_service.h"
#include <string>
#include "yy_log.h"
NS_YY_BEGIN

Win_IOCP_IOService::Win_IOCP_IOService(OnConCallback onCon, OnDisConCallback onDis, OnReadCallback onRead)
	:m_iocp_handle(NULL), m_onConCallback(onCon), m_onDisConCallback(onDis), m_onReadCallback(onRead), m_listen_sd(INVALID_SOCKET)
{

	NetInit();

	//����iocp���
	m_iocp_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if( m_iocp_handle == NULL )
		THROWNEXCEPT("CreateIoCompletionPort error.errno:%d", GetLastError());
}

Win_IOCP_IOService::~Win_IOCP_IOService()
{
	//������
	if(m_iocp_handle)
	{
		PostQueuedCompletionStatus(m_iocp_handle, 0, 0, NULL);
		CloseHandle(m_iocp_handle);
		m_iocp_handle = NULL;
	}

	NetDestroy();
}

void Win_IOCP_IOService::updateCompletionPort(SOCKET sd, UI32 index)
{
	//�����Ӿ������ɶ˿ڰ�
	m_iocp_handle = CreateIoCompletionPort((HANDLE)sd, m_iocp_handle, (DWORD_PTR)index, 0);
	if( m_iocp_handle == NULL )
		THROWNEXCEPT("CreateIoCompletionPort error.errno:%d", GetLastError());
}

void Win_IOCP_IOService::postRecvRequest(UI32 index, UI64 serial)
{
	peer* p=m_peer_manager.getPeer(index);
	if(p->serial!=serial || p->socket==INVALID_SOCKET)
		return;

	//Ͷ�ݶ�����
	IOContext* io_cxt=newIOCxt(ClientIoRead);

	DWORD dwRecvNumBytes = 0;
	DWORD dwFlags = 0;

	p->read_buf.ensureWritableBytes(512);

	WSABUF wsabuf;
	wsabuf.buf=p->read_buf.writeStart();
	wsabuf.len=p->read_buf.writableBytes();


	int nRet = WSARecv(p->socket, &wsabuf, 1, &dwRecvNumBytes, &dwFlags, &io_cxt->overlapped, NULL);
	if( nRet == SOCKET_ERROR)
	{
		int nErrNo=WSAGetLastError();
		if(ERROR_IO_PENDING == nErrNo)
			return;

		//WSAECONNRESET, �Զ˹ر�����; WSAEWOULDBLOCK, �����ж��ش���ͳһ�رո�����
		LOG_WARN("WSARecv error. errno:%d", nErrNo);
		closePeer(index, serial);
	}
}

void Win_IOCP_IOService::postSendRequest(UI32 index, UI64 serial)
{
	peer* socket_p=m_peer_manager.getPeer(index);
	if(socket_p->serial!=serial || socket_p->socket==INVALID_SOCKET)
		return;

	socket_p->wsa_send=true;

	DWORD dwSendNumBytes = 0;
	DWORD dwFlags = 0;

	IOContext* io_p=newIOCxt(ClientIoWrite);

	WSABUF wsabuf;
	wsabuf.buf=socket_p->write_buf.readStart();
	wsabuf.len=socket_p->write_buf.readableBytes();

	int nRet = WSASend(socket_p->socket, &wsabuf, 1, &dwSendNumBytes, dwFlags, (LPWSAOVERLAPPED)io_p, NULL);
	if( nRet == SOCKET_ERROR)
	{
		int nErrNo=WSAGetLastError();
		if(ERROR_IO_PENDING == nErrNo)
			return;

		//WSAECONNRESET, �Զ˹ر�����; WSAEWOULDBLOCK, �����ж��ش���ͳһ�رո�����
		LOG_WARN("WSASend error. errno:%d", nErrNo);
		closePeer(index, serial);
	}
}


bool Win_IOCP_IOService::connectPeer(const char* ip, UI32 port)
{
	//create socket
	SOCKET socketfd =ConnectSocket(m_listen_sd, ip, port);
	if(socketfd == INVALID_SOCKET)
		return false;

	//ע��
	registerSocket(socketfd);
	return true;
}

void Win_IOCP_IOService::listen(const char* ip, UI32 port)
{
	ListenSocket(m_listen_sd, ip, port);
}

void Win_IOCP_IOService::onAcceptTick()
{
	if(m_listen_sd == INVALID_SOCKET)
		THROWNEXCEPT("no socket is listen.");

	//init
	FD_ZERO(&m_read_set);

	//��Ӽ��������read array,��������ӽ���������Ӧ
	FD_SET(m_listen_sd, &m_read_set);

	//select accept event
	timeval timeout;
	timeout.tv_sec=0;		//seconds
	timeout.tv_usec=0;		//micro seconds
	int ready_socket_num = select(	0,					//����
									&m_read_set,		//ָ������е�ָ��
									NULL,
									NULL,
									&timeout);			//��ʱ

	if(ready_socket_num == -1)
		THROWNEXCEPT("select erro.errno:%d", WSAGetLastError());

	//new connection
	if(ready_socket_num>0 && FD_ISSET(m_listen_sd, &m_read_set))
	{
		//����������ӣ����÷�����ioȥaccept,�п���ʧ��
		SOCKET sd;
		if(AcceptSocket(m_listen_sd, sd))
			registerSocket(sd);
	}
}

void Win_IOCP_IOService::send(UI32 index, UI64 serial, const char* buf, UI32 len)
{
	peer* p=m_peer_manager.getPeer(index);
	if(p->serial!=serial || p->socket==INVALID_SOCKET)
		return;

	//��Ҫ���͵����ݷŵ����û�����
	p->write_buf_backup_.apendBuf(buf, len);

	//�����ǰ������û�������ڷ��ͣ�����һ���źţ�����������ڷ��ͣ��������ݷ�����Ϻ���鱸�û���
	if(!p->wsa_send)
	{
		p->wsa_send=true;
		IOContext* io_p=newIOCxt(ClientIoWrite);
		if(!PostQueuedCompletionStatus(m_iocp_handle, 0, (ULONG_PTR)index, (LPOVERLAPPED)io_p))
			THROWNEXCEPT("post erro. errnor:%d", GetLastError());
	}
}

void Win_IOCP_IOService::closePeer(UI32 index, UI64 serial)
{
	peer* p=m_peer_manager.getPeer(index);
	if(!p || p->serial!=serial || p->socket==INVALID_SOCKET)
		return;

	if(m_onDisConCallback)
		m_onDisConCallback(index, serial, p->remote_ip, p->remote_port);

	//��GetQueuedCompletionStatus����false
	CloseSocket(p->socket);
	m_peer_manager.deletePeer(index);
	p=NULL;

}


IOContext* Win_IOCP_IOService::newIOCxt(IO_OPERATION type)
{
	IOContext* io_cxt=m_iocxt_pool.allocElem();
	memset(io_cxt, 0, sizeof(OVERLAPPED));
	io_cxt->io_operation=type;
	return io_cxt;
}

void Win_IOCP_IOService::deleteIOCxt(IOContext* io_cxt)
{
	m_iocxt_pool.freeElem(io_cxt);
}

void Win_IOCP_IOService::registerSocket(SOCKET sd)
{

	UI32 index=m_peer_manager.newPeer();
	peer* p=m_peer_manager.getPeer(index);

	//�󶨵���ɶ˿�
	updateCompletionPort(sd, index);

	//���ip��Ϣ
	struct sockaddr_in remote;
	int remote_len = sizeof(remote);
	getpeername(sd, (struct sockaddr *)&remote, &remote_len);
	strcpy_s(p->remote_ip, sizeof(p->remote_ip), inet_ntoa(remote.sin_addr));
	p->remote_port=ntohs(remote.sin_port);

	struct sockaddr_in local;
	int local_len = sizeof(local);
	getsockname(sd, (struct sockaddr *)&local, &local_len);
	strcpy_s(p->local_ip, sizeof(p->local_ip), inet_ntoa(local.sin_addr));
	p->local_port=ntohs(local.sin_port);

	p->socket=sd;

		//���ûص�����
	if(m_onConCallback)
	{
		m_onConCallback(index, p->serial, p->remote_ip, p->remote_port);
	}

	//Ͷ�ݶ�����
	postRecvRequest(index, p->serial);
}


void Win_IOCP_IOService::processWrite(UI32 index, UI32 size)
{
	peer* p=NULL;
	p=m_peer_manager.getPeer(index);
	if(p->socket==INVALID_SOCKET)
		return;

	//Ͷ�ݵ�����õ���Ӧ��
	p->wsa_send=false;

	//����ѷ��͵�����
	p->write_buf.readMove(size);

	UI32 send_bytes=p->write_buf_backup_.readableBytes();
	if(send_bytes > 0)
	{
		//�ӱ���д�����л�ȡ����
		p->write_buf.apendBuf(p->write_buf_backup_.readStart(), send_bytes);
		p->write_buf_backup_.readMove(send_bytes);
	}

	//�����������Ҫ���ͣ������Ͷ��
	if(p->write_buf.readableBytes() > 0)
		postSendRequest(index, p->serial);
}

void Win_IOCP_IOService::processRead(UI32 index, UI32 size)
{
	peer* p=NULL;

	p=m_peer_manager.getPeer(index);
	if(p->socket==INVALID_SOCKET)
		return;

	//�����յ�������
	p->read_buf.writeMove(size);

	//֪ͨӦ�ò�--���¼�
	if(m_onReadCallback)
	{
		int ret=m_onReadCallback(index, p->serial, &p->read_buf);
		if(ret<0)
		{
			//�Ͽ�����
			closePeer(index, p->serial);
			return;
		}
	}

	//����Ͷ�ݶ�����
	postRecvRequest(index, p->serial);
}

void Win_IOCP_IOService::eventLoop(UI32 count/* =1024 */, UI32 time_out/* =1000 */)
{
	for(UI32 i=0; i<count; i++)
	{
		if(m_listen_sd!=INVALID_SOCKET)
			onAcceptTick();

		//û�����ݣ����������˳������˳���ѭ��
		if(!onEventTick(time_out))
			break;
	}
}

bool Win_IOCP_IOService::onEventTick(UI32 time_out)
{
	IOContext* io_context=NULL;
	peer* p=NULL;
	int index=-1;

	LPOVERLAPPED* lpOverlapped = NULL;
	BOOL bSuccess = FALSE;
	int nRet = 0;
	DWORD dwRecvNumBytes = 0;
	DWORD dwSendNumBytes = 0;
	DWORD dwFlags = 0;
	DWORD dwIoSize = 0;		//������ֽ���

	bSuccess = GetQueuedCompletionStatus(m_iocp_handle, &dwIoSize,(PDWORD_PTR)&index, (LPOVERLAPPED *)&lpOverlapped, time_out);
	if(!bSuccess)
	{
		//time out
		DWORD err_num=GetLastError();
		if(err_num==WAIT_TIMEOUT)
		{
			//ֻ�г�ʱ��ʱ�򷵻�,˵����ʱû����������
			return false;
		}

		//NULL == lpOverlapped, ��lpNumberOfBytes and lpCompletionKey ��δ����ʼ����
		//NULL != lpOverlapped, ��lpNumberOfBytes and lpCompletionKey �б���ʼ��

		//client close socket
		//wsasend/wsarecv are not finished, and the socket is cancelled.
		if(err_num==ERROR_NETNAME_DELETED &&NULL != lpOverlapped)
		{
			p=m_peer_manager.getPeer(index);
			closePeer(index, p->serial);
			io_context=(IOContext*)lpOverlapped;
			deleteIOCxt(io_context);
			return true;
		}


		//�д���ֱ�ӱ���
		THROWNEXCEPT("shoud not be here.");
		if(NULL == lpOverlapped && err_num == ERROR_ABANDONED_WAIT_0)
		{
			//error
		}
	}


	//�������Ϣ�Ĵ���
	p=m_peer_manager.getPeer(index);
	if(p->socket==INVALID_SOCKET)
	{
		//��socket�Ѿ��Ͽ�
		return true;
	}

	io_context=(IOContext*)lpOverlapped;

	//�����˳���ʱ��,������ͨ��
	if(!lpOverlapped && 0 == index && 0 == dwIoSize)
	{
		closePeer(index, p->serial);
		if(io_context)
			deleteIOCxt(io_context);

		return false;
	}

	//�жϾ�����Ϣ
	switch(io_context->io_operation)
	{
	case ClientIoRead:

		//peer shutdown 
		if(0 == dwIoSize)
		{
			closePeer(index, p->serial);
		}
		else
			processRead(index, dwIoSize);
		break;

		//send post, 0 if sending
	case ClientIoWrite:
		processWrite(index, dwIoSize);
		break;

	default:
		THROWNEXCEPT("should not be here.");
		break;
	}

	if(io_context)
	{
		deleteIOCxt(io_context);
	}

	return true;
}

NS_YY_END
