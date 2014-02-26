/************************************************************************/
/*
@author:  junliang
@brief:   ��־�⣬�̰߳�ȫ��ͬ��д�ļ���ʽ��
          ��־���Է�syslog, ҵ�����ݡ�����־����syslog,��ҵ�������Ҿ���
		  Ӧ���е�����ģ��������ʵ�֡���ȻҲ�������첽��ʽ:msgqueue+log thread
		  ѡ��ͬ����ʽ��������д�ļ��Ǻܿ��io�������Ҳ���ҵ�����ݣ����Ƶ��
		  ��д���̡�
*/
/************************************************************************/
#ifndef _LOG_H_
#define _LOG_H_

#include "yy_macros.h"
#include "yy_file.h"
#include "yy_queue.h"
#include "yy_thread.h"
#include "yy_synchronize.h"

NS_YY_BEGIN

#define MAX_LOG_SIZE 1024
#define LOG_ERR(fmt, ...)						\
	Log::getInstance()->logging(Error, fmt, __VA_ARGS__);

#define LOG_WARN(fmt, ...)						\
	Log::getInstance()->logging(Warn, fmt, __VA_ARGS__);

#define LOG_INFO(fmt, ...)						\
	Log::getInstance()->logging(Info, fmt, __VA_ARGS__);

#define LOG_TRACE(fmt, ...)						\
	Log::getInstance()->logging(Trace, fmt, __VA_ARGS__);

#define LOG_DEBUG(fmt, ...)						\
	Log::getInstance()->logging(Debug, fmt, __VA_ARGS__);


enum LogLevel
{
	Error=0,
	Warn,
	Info,
	Trace,
	Debug,
};


enum Color
{
	BLACK=0,
	RED,
	GREEN,
	BROWN,
	BLUE,
	MAGENTA,
	CYAN,
	GREY,
	YELLOW,
	LRED,
	LGREEN,
	LBLUE,
	LMAGENTA,
	LCYAN,
	WHITE
};
const int Color_count = int(WHITE)+1;
const int Level_count = int(Debug)+1;


class Log
{
private:
	struct LogMsg
	{
		LogLevel level;
		char data[MAX_LOG_SIZE];
	};

public:
	Log();
	~Log();
	static Log * getInstance();
	void setLevel(LogLevel level=Info);
	void logging(LogLevel level, const char * fmt, ...);

private:
	void runInThread();
	void logData(LogLevel level, const char * buf);
	void setColor(Color color);
	void resetColor();
	void logToConsole(const char* log);
	void logToFile(const char* log);
	void logToErrFile(const char* log);
private:
	LogLevel m_log_level;		//log����ȼ�
	FILE* m_err_log_file;
	FILE* m_all_log_file;
	UI32 m_log_line;
	MsgQueue<LogMsg> m_queue;
	Thread m_thread;
	CSLock m_lock;
};

NS_YY_END
#endif