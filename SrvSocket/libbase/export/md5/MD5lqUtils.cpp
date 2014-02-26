// MD5lqUtils.cpp
//include "stdafx.h"
#include "MD5lqUtils.h"
#include "MD5lq.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
//#include <Windows.h>

inline bool GetMD5( const char* src, unsigned long size, char md5[32] ) 
{
    long param[4] = { 0x10325476, 0xEFCDAB89, 0x67452301, 0x98BADCFE };

    return MD5( src, md5, size, true, param[0], param[1], param[2], param[3] );
}

// ģ��ReadFile ��д��ȡ�ڴ�
bool ReadMemBuffer(unsigned long nStartReadPos,         // ������ʼλ��
				   unsigned long nEndReadPos,           // ����λ��
                    const char* lpMem,          // ԭʼ�����ڴ��ַ
				   void* lpBuffer,             // ���峤��
				   unsigned long nNumberOfBytesToRead,  // ����BUF����
				   unsigned long *lpNumberOfBytesRead // ʵ�ʶ����ĳ���
)
{
	// Ѱַ����
	if(nStartReadPos > nEndReadPos)
		return false;
	//::ZeroMemory(lpBuffer,nNumberOfBytesToRead);
	memset(lpBuffer, 0, nNumberOfBytesToRead);
    unsigned long nDeffReadPos = nEndReadPos-nStartReadPos;

	// ����ļ��ڴ��ַ��ֵ����λ�úͽ���λ�����С��һ��Buf����,��˵���쵽�ļ�β����
	if( nDeffReadPos <= nNumberOfBytesToRead )
	{
		::memcpy(lpBuffer,lpMem+nStartReadPos, nDeffReadPos );
		*lpNumberOfBytesRead = nDeffReadPos;
	}
	else //���ﶼ�������������ݿ��ȡ
	{
		::memcpy(lpBuffer,lpMem+nStartReadPos, nNumberOfBytesToRead );
		*lpNumberOfBytesRead = nNumberOfBytesToRead;
	}

    return true;
}

// �����ڴ���MD5��
bool GenerateMemInfo( const char* lpMem, unsigned long dwSize, char bufMD5[32] )
{
	if(!lpMem)
		return false;
    const unsigned long BLOCK_SIZE = 10 * 1024 * 1024;//�ļ��ֿ飬ÿ��Ϊ10M
    char *buffer = new char[BLOCK_SIZE];
    if(buffer == NULL) 
        return false;

	bufMD5[0] = '\0';
	buffer[0] = '\0';
    
	bool bRe = true;
    char md5[64 + 1] = {'\0'};
    unsigned long size = BLOCK_SIZE;
	unsigned long start_pos = 0;//��ʼλ��
	while(size == BLOCK_SIZE)	//��ȡһֱ��SIZE���ļ�β��SIZE<=BLOCK_SIZE
	{
		//��ȡһ��BUFFER
		if(::ReadMemBuffer(start_pos,dwSize,lpMem,buffer,BLOCK_SIZE, &size) != true)
		{
			bRe = false;
			break;
		}
		start_pos += BLOCK_SIZE;

		//һ�����ݿ����һ��MD5
        GetMD5( buffer, size, md5 );
		//��ʼ��md5�ĺ�32λΪ��
        ::memcpy( md5 + 32, bufMD5, 32 );
		//�ٰѴ�md5��������һ���µ�md5
        GetMD5( md5, 64, bufMD5 );
	}

	delete []buffer;
    return bRe;
}


bool GenerateFileInfo(FILE* file, char bufMD5[32] )
{
	if( file == NULL )
		return false; 

	const unsigned long BLOCK_SIZE = 10 * 1024 * 1024;
	char *buffer = new char[BLOCK_SIZE];
	if(buffer == NULL) 
		return false;

	bufMD5[0] = '\0';
	buffer[0] = '\0';

	bool bRe = true;
	char md5[64 + 1] = {'\0'};
	unsigned long size = BLOCK_SIZE;

	fseek(file, 0, SEEK_SET);
	while(size == BLOCK_SIZE) 
	{
		size = fread(buffer, 1, BLOCK_SIZE, file);
		if(size == 0)
		{
			bRe = false;
			break;
		}

		GetMD5( buffer, size, md5 );
		::memcpy( md5 + 32, bufMD5, 32 );
		GetMD5( md5, 64, bufMD5 );
	}
	delete []buffer;
	return bRe;
}

//���ļ�����md5��
bool GetMD5FromFile(const char* szFileName, char bufMD5[32])
{

	FILE* file=NULL;
	fopen_s(&file, szFileName, "rb");

	//����ļ�������
	if(file == NULL)
		return false;

	memset(bufMD5, 0, 32);

	if(!GenerateFileInfo( file, bufMD5 ))
	{
		fclose(file);
		return false;
	}

	fclose(file);
	return true;
}