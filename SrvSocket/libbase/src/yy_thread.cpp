#include "yy_thread.h"
#include "yy_exception.h"
#include "yy_log.h"
#include <process.h>


NS_YY_BEGIN


unsigned int __stdcall ThreadFunc(void* param)
{
	Thread* t=(Thread*)param;
	while(!t->stop())
		t->run();

	return 0;
}

Thread::Thread():m_stop_flag(false), m_thread_num(0), m_delegate(NULL)
{}

Thread::Thread(ThreadFunCallback func):m_stop_flag(false), m_thread_num(0), m_delegate(func)
{}

Thread::~Thread()
{
	close();
}

void Thread::bind(ThreadFunCallback func)
{
	m_delegate=func;
}

void Thread::open(UI32 num)
{
	if(!m_delegate)
		THROWNEXCEPT("thread func is null.");

	if(num>=MAX_THRED_NUM)
		THROWNEXCEPT("max thred num:%d, thread num:%d", MAX_THRED_NUM, num);

	m_thread_num=num;
	for(size_t i=0; i<m_thread_num; i++)
	{
		unsigned int threadID;
		m_thread_handle[i]=(HANDLE)_beginthreadex(NULL,				//��ȫ����
													0,				//ջ�ռ�
													ThreadFunc,		//�߳���ں���
													this,			//���ݵĲ���
													0,				//��־λ
													&threadID);		//�߳�ID

		if(0==m_thread_handle[i])
		{
			THROWNEXCEPT("create thread failed.");
		}
	}
}

void Thread::close()
{
	if(0 == m_thread_num)
		return;

	m_stop_flag=true;

	//�ȴ������߳��˳�
	WaitForMultipleObjects((DWORD)m_thread_num, m_thread_handle, TRUE, INFINITE);

	//������Դ
	while(m_thread_num--)
		CloseHandle(m_thread_handle[m_thread_num]);
}


bool Thread::stop()
{
	return m_stop_flag;
}

void Thread::run()
{
	m_delegate();
}

NS_YY_END