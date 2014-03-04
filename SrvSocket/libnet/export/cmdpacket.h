
#ifndef __CG_CMD_PACKET_H__
#define __CG_CMD_PACKET_H__

/* define default packet buffer size */
const int DEFAULT_CMD_PACKET_SIZE = 1024;
/* define max packet buffer size */
const int MAX_CMD_PACKET_SIZE	  = 1024 * 16;
/* define some length used in packet class */
const int BYTE_SIZE  = 1;
const int LONG_SIZE  = 4;
const int SHORT_SIZE = 2;
const int FLOAT_SIZE = 4;
const int LONGLONG_SIZE=8;
class CmdPacket  
{
public:
	CmdPacket();
	virtual ~CmdPacket();

	void BeginWrite();
	void BeginWrite(char const*p,int len);
	void BeginRead(char const*p,int len);
	void BeginRead();

	bool ReadBinary(char **data,int *len);
	bool ReadString(char **str);
	bool ReadFloat(float *f);
	bool ReadInt(int *l);
	bool ReadUInt(UINT32 *l);
	bool ReadInt64(INT64 *l);
	bool ReadUInt64(UINT64 *l);
	bool ReadShort(short *s);
	bool ReadUShort(u_short *s);
	bool ReadByte(char *c);

	bool WriteBinary(char *data,int len);
	bool WriteString(char *str);
	bool WriteFloat(float f);
	bool WriteInt(int l);
	bool WriteUInt(UINT32 l);
	bool WriteInt64(INT64 l);
	bool WriteUInt64(UINT64 l);
	bool WriteShort(short s);
	bool WriteUShort(u_short s);
	bool WriteByte(char c);

	char *GetData();
	int   GetDataSize();
	int   GetMaxSize();

	bool  SetSize(int len);
	bool  CloneFrom(CmdPacket *packet);
	
	char *GetReadData();
private :
	bool  CopyData(char const*buf,int len);
	bool  WriteData(void *data,int len);
	bool  ReadData(void *data,int len);

	char *m_pData;
	char *m_pReadData;
	int   m_nLen;
	int   m_nReadOffset;
	int   m_nWriteOffset;
	int   m_nMaxSize; 
};

#endif