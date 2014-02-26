#include "Select_IOService.h"
#include <algorithm>
#include "YYException.h"
#include "YYLog.h"
NS_YY_BEGIN

SelectIOService::SelectIOService(OnConCallback onCon, OnDisConCallback onDis, OnReadCallback onRead)
	:m_onConCallback(onCon), m_onDisConCallback(onDis), m_onReadCallback(onRead), m_listen_sd(INVALID_SOCKET)
{
	NetInit();
}

SelectIOService::~SelectIOService()
{
	NetDestroy();
}

void SelectIOService::registerSocket(SOCKET socket)
{
	UI32 index=m_peer_manager.newPeer();
	peer* p=m_peer_manager.getPeer(index);

	//���ip��Ϣ
	struct sockaddr_in remote;
	int remote_len = sizeof(remote);
	getpeername(socket, (struct sockaddr *)&remote, &remote_len);
	strcpy_s(p->remote_ip, sizeof(p->remote_ip), inet_ntoa(remote.sin_addr));
	p->remote_port=ntohs(remote.sin_port);

	struct sockaddr_in local;
	int local_len = sizeof(local);
	getsockname(socket, (struct sockaddr *)&local, &local_len);
	strcpy_s(p->local_ip, sizeof(p->local_ip), inet_ntoa(local.sin_addr));
	p->local_port=ntohs(local.sin_port);

	//��ʼ��
	p->socket=socket;
	m_online_peers.push_back(index);

	if(m_onConCallback)
	{
		m_onConCallback(index, p->serial, p->remote_ip, p->remote_port);
	}
}

bool SelectIOService::connectPeer(const char* ip, UI32 port)
{
	//create socket
	SOCKET socketfd = ConnectSocket(m_listen_sd, socketfd, ip, port);
	if(socketfd == INVALID_SOCKET)
		return false;

	registerSocket(socketfd);
	return true;
}

void SelectIOService::listen(const char* ip, UI32 port)
{
	ListenSocket(m_listen_sd, ip, port);
}


void SelectIOService::send(UI32 index, UI64 serial, const char* buf, UI32 len)
{
	//�жϺϷ���,�п��ܸ������ѶϿ�
	peer* p=m_peer_manager.getPeer(index);
	if(p && p->serial!=serial || p->socket<=0)
		return;

	//insert into send buffer
	p->write_buf.apendBuf(buf, len);
}

void SelectIOService::closePeer(UI32 index, UI64 serial)
{
	peer* p=m_peer_manager.getPeer(index);

	//1,�Ѿ��ر���״̬�Ĺ���2���Ѿ��رգ��ұ��������ռ��
	if(p->serial!=serial || p->socket==INVALID_SOCKET)
		return;

	if(m_onDisConCallback)
		m_onDisConCallback(index, serial, p->remote_ip, p->remote_port);

	std::vector<UI32>::iterator itor=find(m_online_peers.begin(), m_online_peers.end(), index);
	if(itor!=m_online_peers.end())
		m_online_peers.erase(itor);

	CloseSocket(p->socket);
	m_peer_manager.deletePeer(index);
	p=NULL;
}

void SelectIOService::eventLoop(UI32 count/* =1024 */, UI32 time_out/* =1000 */)
{
	for(UI32 i=0; i<count; i++)
	{
		//û�����ݣ����������˳������˳���ѭ��
		if(!onEventTick(time_out))
			break;
	}
}

bool SelectIOService::onEventTick(UI32 time_out)
{
	//���û�о���������򷵻�
	if(m_online_peers.empty() && m_listen_sd == INVALID_SOCKET)
		return false;

	peer* tmp=NULL;

	//��ʼ�����
	initSets();

	//�߳����������û��Ϣ���򷵻ء�������nullһֱ������Ϊ�˷�ֹ�����˳���ʱ���io�߳��޷�����
	timeval timeout;
	timeout.tv_sec=time_out/1000;			//seconds
	timeout.tv_usec=(time_out%1000) * 1000;	//micro seconds

	//��������socket
	int ready_socket_num = select(	0,					//����
									&m_read_set,		//ָ������е�ָ��
									&m_write_set,		//ָ��д���е�ָ��
									NULL,
									&timeout);			//��ʱ

	int err_num=WSAGetLastError();
	if(ready_socket_num == -1)
		THROWNEXCEPT("select erro.errno:%d", WSAGetLastError());

	//�����socket��״̬�����仯
	if(ready_socket_num>0)
	{
		if(m_listen_sd != INVALID_SOCKET)
		{
			//��������
			if(FD_ISSET(m_listen_sd, &m_read_set))
			{
				SOCKET sd;
				if(AcceptSocket(m_listen_sd, sd))
					registerSocket(sd);
			}

			//����
			//if(FD_ISSET(m_listen_sd, &exception_set_))
			//{
			//	LOG_ERR("listen socket exception:%d", err_no);
				//continue;
			//}
		}

		//����жϿ����ӣ���������ѭ������ֹm_online_peers�����������ӵ����ݿ��Եȴ���һ��select
		for(size_t i=0; i<m_online_peers.size(); i++)
		{
			UI32 index=m_online_peers[i];
			peer* p=m_peer_manager.getPeer(index);

			//���¼�
			if(FD_ISSET(p->socket, &m_read_set))
			{
				p->read_buf.ensureWritableBytes(512);

				int ret=0;
				ret= ::recv(p->socket, p->read_buf.writeStart(), p->read_buf.writableBytes(), 0);
				if(ret==0)
				{
					//�Զ˵���closesocket api�Ͽ�����
					closePeer(index, p->serial);
					break;
				}

				if(ret<0)
				{
					//WSAECONNRESET, �Զ˹ر�����; WSAEWOULDBLOCK, �����ж��ش���ͳһ�رո�����
					LOG_WARN("recv error. errno:%d", WSAGetLastError());
					closePeer(index, p->serial);
					break;
				}

				p->read_buf.writeMove(ret);
				if(m_onReadCallback(index, p->serial, &p->read_buf) < 0)
				{
					//�Ͽ�����
					closePeer(index, p->serial);
					break;
				}
			}

			//д�¼�
			if(FD_ISSET(p->socket, &m_write_set))
			{
				//д��������������Ҫ����
				int readable_byets=p->write_buf.readableBytes();
				if(readable_byets>0)
				{
					int bytes=0;
					bytes=::send(p->socket, p->write_buf.readStart(), p->write_buf.readableBytes(), 0);
					if(bytes<0)
					{
						//WSAECONNRESET, �Զ˹ر�����; WSAEWOULDBLOCK, �����ж��ش���ͳһ�رո�����
						LOG_WARN("send error. errno:%d", WSAGetLastError());
						closePeer(index, p->serial);
						break;
					}

					p->write_buf.readMove(bytes);
				}
			}
		}
		
		return true;
	}
	else if(ready_socket_num == 0)
	{
		//timeout, ֻ�г�ʱ��ʱ�򷵻�false,˵����ǰû������������
		return false;
	}

	return false;
}

void SelectIOService::initSets()
{
	//����������еľ���������
	FD_ZERO(&m_read_set);
	FD_ZERO(&m_write_set);

	if(m_listen_sd != INVALID_SOCKET)
	{
		//��Ӽ��������read array,��������ӽ���������Ӧ
		FD_SET(m_listen_sd, &m_read_set);
		//FD_SET(m_listen_sd, &exception_set_);
	}

	std::vector<UI32>::iterator itor=m_online_peers.begin();
	for(; itor!=m_online_peers.end(); itor++)
	{
		UI32 index=*itor;
		peer* p=m_peer_manager.getPeer(index);

		//������Ҫ����(tcp��ȫ˫���ģ�����ͬʱ���ͽ�������)
		if(p->write_buf.readableBytes()>0)
		{
			FD_SET(p->socket, &m_write_set);
		}

		//��ӵ�read array
		FD_SET(p->socket, &m_read_set);
	}
}

NS_YY_END