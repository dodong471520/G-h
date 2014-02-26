#ifndef _PEER_MANAGER_HPP_
#define _PEER_MANAGER_HPP_

#include "sockbuf.h"
#include <windows.h>
NS_YY_BEGIN

#define MAX_PEER_NUM 1024*5	//���������



//�ڵ���Ϣ
struct peer
{
	SOCKET socket;
	char local_ip[50];
	UI32 local_port;
	char remote_ip[50];
	UI32 remote_port;

	UI64 serial;				//���кţ�ÿ�������������ɵ����кţ�session key��Ч��
	bool wsa_send;				//��֤һ��socketֻ��һ��sendͶ��, recvֻ��io thread���������Կ��Ա�ֻ֤��һ��recvͶ��

	SockBuf read_buf;
	SockBuf write_buf;			//wsa buf, Ͷ��֮�����ϵͳ�������ڴ棬����Ͷ��֮��Ͳ����޸Ļ�������
	SockBuf write_buf_backup_;	//����Ѿ�Ͷ��д������û��汣��Ҫ���͵�����

	peer()
	{
		wsa_send=false;
		socket=INVALID_SOCKET;
		memset(local_ip, 0, sizeof(local_ip));
		local_port=0;
		memset(remote_ip, 0, sizeof(remote_ip));
		remote_port=0;
		serial=0;
	}
};

//�����鷽ʽ�����������ӽڵ�, ���̰߳�ȫ
class PeerManager
{
public:
	PeerManager();
	~PeerManager();

	UI32 newPeer();
	void deletePeer(UI32 index);

	peer* getPeer(UI32 index);

private:
	peer* m_peer_array;
	UI64 m_serial;
};

NS_YY_END
#endif