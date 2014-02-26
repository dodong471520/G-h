#ifndef _WIN_LOCK_H_
#define _WIN_LOCK_H_

#include "yy_macros.h"
#include <Windows.h>

NS_YY_BEGIN

//�û�ģʽ�µ��߳�ͬ��:�ؼ���
class CSLock
{
public:
	CSLock()
	{
		InitializeCriticalSection(&m_cs);
	}
	~CSLock()
	{
		DeleteCriticalSection(&m_cs);
	}

	void lock()
	{
		EnterCriticalSection(&m_cs);
	}

	void unlock()
	{
		LeaveCriticalSection(&m_cs);
	}

private:
	CRITICAL_SECTION m_cs;
};

//�ں˶�����߳�ͬ��:�¼�
class CEvent
{
public:
	CEvent(bool initial_state=false, bool manual_reset=false)
	{
		//�����¼�
		event_handle_=::CreateEvent(NULL,	//��ȫ����
			manual_reset,					//�Ƿ��Զ�����
			initial_state,					//�Ƿ��ʼ��Ϊδ����״̬
			NULL);							//����
	}

	~CEvent()
	{
		CloseHandle(event_handle_);
	}

	//�����¼�
	int setEvent()
	{
		return ::SetEvent(event_handle_);
	}

	//�ȴ����������¼�
	int pulseEvent() 
	{ 
		return ::PulseEvent(event_handle_); 
	}

	//�����¼�
	int resetEvent() 
	{ 
		return ::ResetEvent(event_handle_); 
	}

	int wait(DWORD dwTimeOut = INFINITE) 
	{
		return ::WaitForSingleObject(event_handle_, dwTimeOut);
	}

	//�жϵ�ǰ�¼��Ƿ񱻴���
	bool isSetEvent()
	{
		return (wait(0) == WAIT_OBJECT_0);
	}

private:
	HANDLE event_handle_;
};


//�ں˶�����߳�ͬ��:�ź���

//�ں˶�����߳�,����ͬ��:������
class CMutex : public ILock
{
public:
    CMutex(char* name)
	{
		createMyMutex(name);
	}

    ~CMutex()
	{
		WaitForSingleObject(m_mutex, 1000);
	}
  
    void createMyMutex(char* name)
	{
		m_mutex = CreateMutexA(NULL, FALSE, name);
		if(m_mutex == NULL)  
		{  
			//create error
			THROWNEXCEPT("create mutex error.");
		}  
	}
    void lock()
	{
		DWORD dw=WaitForSingleObject(m_mutex, 5000);
		if(dw!=WAIT_OBJECT_0)
		{
			//timeout
			THROWNEXCEPT("can not lock.");
		}

	}
    void unlock()
	{
		ReleaseMutex(m_mutex);
	}
  
private:
    HANDLE m_mutex;
};*/

NS_YY_END
#endif
