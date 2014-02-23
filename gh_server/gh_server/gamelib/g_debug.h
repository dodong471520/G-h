/*
g_debug.h: interface for the CG_Debug class.
*/

#ifndef __CG_DEBUG_H__
#define __CG_DEBUG_H__

#include "g_platform.h"
#include <fstream>

const int DEFAULT_LOG_BUF_SIZE = 1024;

class CG_Debug  
{
public:
	void Log(char *format,...);
	void SetOpt(bool file,bool print,bool debugstr,bool time,bool idx);
	bool BeginLog(char *name);
	CG_Debug();
	virtual ~CG_Debug();

protected:
	fstream m_file;
	bool m_bFile;
	bool m_bPrint;
	bool m_bDebugString;
	bool m_bTime;
	bool m_bIdx;
	bool m_bInit;
	
	int   m_idx;
	char  m_buf[DEFAULT_LOG_BUF_SIZE];
};

#endif 