#ifndef _m_queueH_
#define _m_queueH_

#include "yy_memory.h"
#include "yy_exception.h"
#include <string.h>
#include <Windows.h>
#include <stdlib.h>


NS_YY_BEGIN

//�̰߳�ȫ�� ǳ������ ������Ϣ���У����Ż��ɻ��ζ���
template<class T>
class MsgQueue
{
public:
	MsgQueue(size_t max_size=1024);
	~MsgQueue();

	void append(const T* data, DWORD timeout=0);
	void remove(T* data, DWORD dwTimeout=0);
private:
	HANDLE m_handles[2];			//[0]: mutex,�������ں˶���; [1]: semaphone���ź����ں˶���
	size_t m_max_size;				//�������Ԫ�صĴ�С
	T* m_queue;						//���Ԫ�صĶ���ָ��
};


template<class T>
MsgQueue<T>::MsgQueue(size_t max_size)
{
	m_max_size=max_size;
	m_queue=(T*)MPAlloc(m_max_size*sizeof(T));

	//����������
	m_handles[0]=CreateMutex(NULL, false, NULL);

	//�����ź���
	m_handles[1]=CreateSemaphore(NULL, 0, (long)m_max_size, NULL);
}

template<class T>
MsgQueue<T>::~MsgQueue()
{
	SAFE_DELETE_ARRAY(m_queue);
}

template<class T>
void MsgQueue<T>::append(const T* data, DWORD timeout)
{
	DWORD dw=WaitForSingleObject(m_handles[0], timeout);
	if(dw==WAIT_OBJECT_0)
	{
		//�����ź����ĵ�ǰ��Դ����
		LONG prev_count;
		if(FALSE == ReleaseSemaphore(m_handles[1], 1, &prev_count))
		{
			ReleaseMutex(m_handles[0]);

			//��������ֵ����
			THROWNEXCEPT("append to queue error, the queue is full");
		}

		memcpy(&m_queue[prev_count], data, sizeof(T));
		ReleaseMutex(m_handles[0]);
	}
	else
	{
		//timeout
		THROWNEXCEPT("append to queue timeout.");
	}
}

template<class T>
void MsgQueue<T>::remove(T* data, DWORD dwTimeout)
{
	DWORD dw=WaitForMultipleObjects(_countof(m_handles), m_handles, TRUE, dwTimeout);
	if(dw==WAIT_OBJECT_0)
	{
		memcpy(data, &m_queue[0], sizeof(T));

		//���ֽ��Կ���
		memmove(&m_queue[0], &m_queue[1], sizeof(T)*(m_max_size-1));
		ReleaseMutex(m_handles[0]);
	}
	else
	{
		//timeout
		THROWNEXCEPT("remove from queue timeout");
	}
}

NS_YY_END
#endif