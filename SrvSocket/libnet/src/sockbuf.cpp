#include "sockbuf.h"
#include <stdlib.h>
#include <string.h>
#include "yy_memory.h"
#include "yy_exception.h"
NS_YY_BEGIN

SockBuf::SockBuf(const UI32 size)
{
	m_size=size;
	m_buf=(char*)MPAlloc(m_size);
	m_read_index=0;
	m_write_index=0;
}

SockBuf::~SockBuf()
{
	MPFree(m_buf);
}

void SockBuf::reset()
{
	m_read_index=0;
	m_write_index=0;
}

char* SockBuf::writeStart()
{
	return m_buf+m_write_index;
}

char* SockBuf::readStart()
{
	return m_buf+m_read_index;
}

void SockBuf::readMove(UI32 len)
{
	m_read_index+=len;
	if(m_read_index > m_write_index)
		THROWNEXCEPT("write index:%u is not bigger than read index:%u", m_write_index, m_read_index);
}

void SockBuf::writeMove(UI32 len)
{
	m_write_index+=len;
	if(m_write_index > m_size)
		THROWNEXCEPT("write index is out of range.");
}

UI32 SockBuf::readableBytes()
{
	int nret=m_write_index - m_read_index;
	if(nret < 0)
		THROWNEXCEPT("write index:%u is not bigger than read index:%u", m_write_index, m_read_index);

	return nret;
}

UI32 SockBuf::writableBytes()
{
	int nret= m_size - m_write_index;
	if(nret < 0)
		THROWNEXCEPT("write index:%u is bigger than size:%u", m_write_index, m_size);

	return nret;
}

void SockBuf::makeSpace(UI32 len)
{
	UI32 readable_bytes=readableBytes();

	//�ռ䲻�����򿪱�
	if (writableBytes() + m_read_index < len)
	{
		//�����¿ռ�
		char* new_buf=(char*)MPAlloc(m_write_index+len);
		m_size=m_write_index+len;

		//�����ɶ�����
		memcpy(new_buf, m_buf+m_read_index, readable_bytes);
		m_read_index=0;
		m_write_index=readable_bytes;

		//����͵�ַ
		char* old_buf=m_buf;
		//ָ���¿ռ�
		m_buf=new_buf;

		//���վɿռ�
		if(old_buf)
			MPFree(old_buf);
		old_buf=NULL;
	}
	//�ڲ���Ų
	else
	{
		//�Կ���
		memmove(m_buf, m_buf+m_read_index,readable_bytes);
		m_read_index=0;
		m_write_index=readable_bytes;
	}
}

void SockBuf::ensureWritableBytes(UI32 len)
{
	if (writableBytes() < len)
	{
		makeSpace(len);
	}
}

void SockBuf::apendBuf(const char* buf, UI32 len)
{
	//1,�жϴ�С��������������¿���
	ensureWritableBytes(len);

	//2������
	memcpy(m_buf+m_write_index, buf, len);
	m_write_index += len;
	if(m_write_index > m_size)
		THROWNEXCEPT("write index:%u is bigger than size:%u", m_write_index, m_size);

}

NS_YY_END

