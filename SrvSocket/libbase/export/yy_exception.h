/************************************************************************/
/*
@author:  junliang
@brief:   exception
@time:    20130401
*/
/************************************************************************/
#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include "yy_macros.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
NS_YY_BEGIN

//class Exception;
#define MAXEXCEPTIONSIZE 1024
#define THROWNEXCEPT(fmt, ...)		throw Exception(__FILE__, __LINE__, __FUNCTION__, fmt, __VA_ARGS__);

class Exception
{
public:
	Exception(const char* file, int line, const char* func, const char* fmt, ...)
	{
		size_t len = 0;
		UI32 array_size=sizeof(m_buf);

		if(NULL != file)
		{
			sprintf_s(m_buf, sizeof(m_buf), "[%s, line:%d, %s] ", file, line, func);
			len = strlen(m_buf);
		}

		va_list argptr;
		va_start(argptr, fmt);
		vsprintf_s(m_buf+len, array_size-len-1, fmt, argptr);
		va_end(argptr);

		m_buf[array_size-1]=0;
	}

	const char* what() const
	{
		return m_buf;
	}

private:
	char m_buf[MAXEXCEPTIONSIZE];
};


NS_YY_END
#endif


/*
ÿ���̺߳�����Ҫ��׽���Ե��쳣

assert vs exception:
1, assert��release���޷�ʧЧ
2, assert�����ʱ��ֻ��ʾ���ʽ����exception�����Զ������������Ϣ, �������Ŵ�

ͨ������һ��ȫ�ֵļ��ϣ������溯�����ù�ϵ����stack trace, �̲߳���ȫ����ʹ������˵��͸��
struct SourcePoint
{
    const char *filename;
    int line;
    SourcePoint(const char *filename, int line)
      : filename(filename), line(line)
    { }
};

std::vector<SourcePoint> callstack;

struct SourcePointMarker
{
    SourcePointMarker(const char *filename, int line)
    {
        callstack.push_back(SourcePoint(filename, line));
    }

    ~SourcePointMarker()
    {
        callstack.pop_back();
    }
};

#define MARK_FUNCTION   SourcePointMarker sourcepointmarker(__FILE__, __LINE__);

*/