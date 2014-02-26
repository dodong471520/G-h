#ifndef _MEMORY_H__
#define _MEMORY_H__


#include "yy_macros.h"
#include <stdlib.h>

NS_YY_BEGIN

//c���Է�ʽ�����ڴ棬������ù��캯������������
class MemoryPool
{
public:
	static MemoryPool* getInstance();
	void* mpAlloc(UI32 size);
	void mpFree(void* p);
};

//c++��ʽ�����ڴ�, ����ù��캯��������������new/delete�ķ�װ,ͨ���̳з�ʽ��ʹ�á�
class NewAlloc
{
public:
	void* operator new(size_t size);
	void operator delete(void* p);

	//new[]�������еĸ�������������Ĵ�С���϶���Ĵ洢������Ŀ��һЩ�ֽڡ�������������������
	void* operator new[](size_t size);
	void operator delete[](void* p);
};

NS_YY_END

//��װmalloc, free api. ����Ժ�Ҫ�Ż��ڴ���������޸ĸ�ʵ��
#define MPAlloc(size)			MemoryPool::getInstance()->mpAlloc(size);
#define MPFree(p)				if(p){MemoryPool::getInstance()->mpFree(p); p=NULL;}

#endif
