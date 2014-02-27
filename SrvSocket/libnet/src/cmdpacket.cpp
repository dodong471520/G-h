/*
g_CmdPacket.cpp: implementation of the CG_CmdPacket class.
*/

#include "../export/cmdpacket.h"
#include <stdlib.h>
#include <memory>
CmdPacket::CmdPacket()
{
	m_nMaxSize = 0;
	m_nLen = 0;
	m_nReadOffset  = 0;
	m_nWriteOffset = 0;
	m_pData = 0;
	/* set buffer to default size */
	SetSize(DEFAULT_CMD_PACKET_SIZE);
}

CmdPacket::~CmdPacket()
{
	delete []m_pData;
}

bool CmdPacket::SetSize(int len)
{
	if(len > MAX_CMD_PACKET_SIZE) return false;
	delete []m_pData;
	m_pData = 0;
	m_pData = new char[len];
	m_nMaxSize = len;
	return m_pData?true:false;
}

/* try write data to packet */
bool CmdPacket::WriteData(void *data, int len)
{
	if ((m_nLen + len) > m_nMaxSize) return false;
	memcpy(m_pData+m_nWriteOffset,data,len);
	m_nLen += len;
	m_nWriteOffset += len;
	return true;
}

bool CmdPacket::WriteByte(char c)
{
	return WriteData(&c,BYTE_SIZE);
}

void CmdPacket::BeginRead(char const*p,int len)
{
	m_pReadData=p;
	m_nLen = len;
	m_nReadOffset = 0;
}

void CmdPacket::BeginRead()
{
	m_pReadData = m_pData;
	m_nReadOffset = 0;
}

void CmdPacket::BeginWrite()
{
	m_nLen = 0;
	m_nWriteOffset = 0;
}

bool CmdPacket::WriteBinary(char *data, int len)
{
	if (WriteShort(len)==false) 
		return false;
	return WriteData(data,len);
}

bool CmdPacket::ReadBinary(char **data, int *len)
{
	short dataLen;
	if (!ReadShort(&dataLen)) return false;
	if (dataLen <= 0) return false;
	if ((m_nReadOffset + dataLen) > m_nLen) return false;
	*data = m_pReadData + m_nReadOffset;
	*len = dataLen;
	m_nReadOffset += dataLen;
	return true;
}

bool CmdPacket::WriteShort(short s)
{
	return WriteData(&s,SHORT_SIZE);
}

bool CmdPacket::WriteLong(long l)
{
	return WriteData(&l,LONG_SIZE);
}

bool CmdPacket::WriteFloat(float f)
{
	return WriteData(&f,FLOAT_SIZE);
}

bool CmdPacket::ReadByte(char *c)
{
	return ReadData(c,BYTE_SIZE);
}

bool CmdPacket::ReadShort(short *s)
{
	return ReadData(s,SHORT_SIZE);
}

bool CmdPacket::ReadLong(long *l)
{
	return ReadData(l,LONG_SIZE);
}

bool CmdPacket::ReadFloat(float *f)
{
	return ReadData(f,FLOAT_SIZE);
}

bool CmdPacket::ReadString(char **str)
{
	short len;
	if (!ReadShort(&len)) return false;
	if (len <= 0) return false;
	if ((m_nReadOffset + len) > m_nLen) return false;
	/* set end avoid overflow */
	*(m_pReadData+m_nReadOffset+len-1) = '\0';
	*str = m_pReadData + m_nReadOffset;
	m_nReadOffset += len;
	return true;
}

bool CmdPacket::WriteString(char *str)
{
	short len = strlen(str) + 1;
	if (!WriteShort(len)) return false;
	return WriteData(str,len);
}

bool CmdPacket::ReadData(void *data, int len)
{
	if ((m_nReadOffset + len)>m_nLen) return false;

	memcpy(data,m_pReadData+m_nReadOffset,len);
	m_nReadOffset += len;
	return true;
}

bool CmdPacket::CopyData(char *buf,int len)
{
	if(!SetSize(len)) return false;
	memcpy(m_pData,buf,len);
	m_nLen = len;
	return true;
}

bool CmdPacket::CloneFrom(CmdPacket *packet)
{
	return CopyData(packet->GetData(),packet->GetDataSize());
}

char *CmdPacket::GetData()
{
	return m_pData;
}

int CmdPacket::GetDataSize()
{
	return m_nLen;
}

int CmdPacket::GetMaxSize()
{
	return m_nMaxSize;
}

