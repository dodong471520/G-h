/*
g_debug.cpp: implementation of the CG_Debug class.
*/

#include "g_debug.h"

CG_Debug::CG_Debug()
{
   m_bInit = false;
   SetOpt(true,true,true,true,true);
   m_idx = 0;
}

CG_Debug::~CG_Debug()
{

}

bool CG_Debug::BeginLog(char *name)
{
	char buf[128],tmpTime[32],tmpDate[32];
	_strtime(tmpTime);
    _strdate(tmpDate);
 
	sprintf(buf,"Log File Create At [%s %s]\r\n",tmpDate,tmpTime);
	m_file.open(name,ios::out);
	if(!m_file.good()) return false;
	m_bInit = true;
	m_file.write(buf,strlen(buf));
	m_file.flush();
	return true;
}

void CG_Debug::SetOpt(bool file, bool print, bool debugstr,bool time,bool idx)
{
	m_bFile = file;
	m_bPrint = print;
	m_bDebugString = debugstr;
	m_bTime = time;
	m_bIdx = idx;
}

void CG_Debug::Log(char *format,...)
{
	if (!m_bInit) return;
	m_idx++;

	int pos = 0;
	if(m_bIdx)
	{
		pos += sprintf(m_buf+pos,"[%5d]",m_idx);
	}
	if(m_bTime)
	{
		char tmpTime[32];
		_strtime(tmpTime);
		pos += sprintf(m_buf+pos,"[%s]",tmpTime);
	}
	
	va_list args;
	va_start(args, format);
	pos += VSNPRINTF(m_buf+pos,DEFAULT_LOG_BUF_SIZE-50,format, args);
	m_buf[pos]	 = '\r';
	m_buf[pos+1] = '\n';
	m_buf[pos+2] = '\0';
#ifdef WIN32
	if(m_bDebugString) OutputDebugString(m_buf);
#endif
	if(m_bPrint) printf("%s",m_buf);
	if(m_bFile)
	{
		m_file.write(m_buf,pos+2);
		m_file.flush();
	}
	va_end(args);
}
