/************************************************************************/
/*
@author:  junliang
@brief:   win select io(select, iocp), ��Ԥ����������ѡ��
			Ĭ����iocpģʽ, ��Ԥ��������IO_SELECT,����selectģʽ
*/
/************************************************************************/
#ifndef _IO_SERVICE_H_
#define _IO_SERVICE_H_


#include "sockbuf.h"
#include "FastDelegate.h"
using namespace fastdelegate;
NS_YY_BEGIN



//�ж�����ģʽ
#ifdef IO_SELECT
	class SelectIOService;
	typedef SelectIOService IOServiceImpl;
#else
	class Win_IOCP_IOService;
	typedef Win_IOCP_IOService IOServiceImpl;
#endif


//�ص��ӿ�, ֻ��io thread������Щ���������̰߳�ȫ
typedef FastDelegate4<UI32, UI64, const char*, UI32> OnConCallback;
typedef FastDelegate4<UI32, UI64, const char*, UI32> OnDisConCallback;
typedef FastDelegate3<UI32, UI64, SockBuf*, int> OnReadCallback;


class IOService
{
public:
	IOService(OnConCallback onCon, OnDisConCallback onDis, OnReadCallback onRead);
	~IOService();

	//����connect
	bool connectPeer(const char* ip, UI32 port);

	//listen for the peer connect
	void listen(const char* ip, UI32 port);

	//send data to peer, ��������ѶϿ�����������Ϣ
	void send(UI32 index, UI64 serial, const char* buf, UI32 len);

	//close peer,�׽����ϲ����ٷ������ͻ�������󣻷��ͻ����е����ݱ����͵��Զˡ����ջ����������ݱ�������������������ü���Ϊ0��
	//�ڷ����귢�ͻ������е����ݺ󣬸���������tcp������ֹ����(������FIN)
	void closePeer(UI32 index, UI64 serial);

	//����count��һ֡�����Խ��ܻ��߷��Ͷ��ٴ���������ô�����֡���ȶ������ᵼ��һ֡ʱ�������
	//����timeout�ǿ���һ�������¼��ĳ�ʱʱ��
	void eventLoop(UI32 count=1024, UI32 time_out=1000);
private:
	IOServiceImpl* m_impl;
};

NS_YY_END
#endif