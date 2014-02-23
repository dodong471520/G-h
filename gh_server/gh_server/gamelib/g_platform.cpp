#include "g_platform.h"

void Sys_Log(char *format,...)
{
	va_list args;
	va_start(args, format);
	char buf[256];
	VSNPRINTF(buf,255,format,args);
    printf("%s\r\n",buf);
	/* write to file */
	va_end(args);
}

unsigned long Sys_GetTime()
{
#ifdef WIN32
	return GetTickCount();
#else 
	/* linux */
	timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec*1000+tv.tv_usec/1000);
#endif
}

unsigned long Sys_GetSec()
{
	return time(NULL);	
}

void Sys_Sleep(int ms)
{
#ifdef WIN32
	Sleep(ms);
#else 
	/* linux */
	usleep(ms*1000);
#endif
}

void Sys_CreateConsole(int width, int height)
{
#ifdef WIN32
	HANDLE hStdOut;
    AllocConsole();
    SetConsoleTitle("Debug Window");
    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE); 
    COORD co = {width , height};
    SetConsoleScreenBufferSize(hStdOut, co); 
    freopen("CONOUT$","w+t",stdout);
    freopen("CONIN$","r+t",stdin);
#endif
}
