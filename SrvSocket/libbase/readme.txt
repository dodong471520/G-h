
Ŀ��: ��ƽ̨������⣬��װwin32, posix api, ֧�ֻ���libbase������Ӧ�ó����ƽ̨��
��Ŀ�ܹ�: ����ͷ�ļ�Ϊ��ƽ̨ͷ�ļ�������ʵ�ַ���win32��posixĿ¼�С�����Impl��ʽʵ�ֿ�ƽ̨
������ʽ:
	�ļ�����YYǰ׺����ֹ��ϵͳ�ļ�����������include "time.h"����������ȥ��ϵͳ�ļ�����������Ŀ�ļ�
	����������ĸ��д
	���Ա����������ĸСд
	ȫ�ֺ���������ĸ��д

�����ΰ�ȫ���档ǿ���Լ���strcpy�ĳ�strcpy_s, �����Խ�磬��ᷢ��assert���󡣸��˾��ñ���strncpy�Ľ�ȡ��ʽ�á�
1,��ȡ��˵���϶��߼��������⡣
2,strncpy�Ļ������Ҫ�����������٣������0(waste time)



�ű�: ��lua-5.1.4�汾������lua tinker����װ
net: ����windows iocp�� ���������߳������������շ���ȡ��һ��������Ϣ�����뵽��Ϣ���С�
serv: ���������̴߳���Ϣ����ȡ��Ϣ������
��ʱ���� �����߳�onTick()��ʵ�֡���֤���߳�������



#ifdef ֮���Ԥ������ֻ��ͷ�ļ�������£���main��#define����Ч����ȻҪ�ڱ�������Ԥ������������Ӻ����Ч.�����õ�Ԥ�������ļ�������ͷ�ļ���ʵ��.
-->head only lib



��ƽ̨������
�����������룬�ѱ���������ͬ�Ĵ���ŵ��������ļ��С�����ͬƽ̨�ĳ����ɲ�ͬ���ļ�(pimpl)
1,log--log impl--win32 log impl, linux log impl
2,log--win32 log impl, linux log impl(����typedef),Ŀǰ�����ַ������ٶ���һ��log impl,��pimpl�ĺô�(�����Ƽ��ݣ�ͷ�ļ���ʵ���ļ�����)��û�м���

���ϲ�ӿ�
//Log.h
#ifdef win32
	class Win_Log;
	typedef LogImpl Win_Log;
#endif

class Log
{
public:
	Log();
	~Log();
	void logging(const char* data);
private:
LogImpl* m_impl;
}

//Log.cpp

#ifdef win32
#include "win32/win_log.h"
#endif

Log::Log() : m_impl(new LogImpl)
{}

Log::~Log()
{ delete m_impl;}

void Log::logging(const char* data){}

//windows
class Win_Log
{
public:
	void logging(const char* data);
}