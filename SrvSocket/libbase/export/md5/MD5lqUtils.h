// MD5lqUtils.h

#ifndef LQINC_MD5LQUTILS_H
#define LQINC_MD5LQUTILS_H

/************************************************************************/
/*
md5�����Ľӿ�,���ǵ����ļ������Էֿ����������һ���MD5�����±����ȥ��
�����������������MD5���ͨ�õ�MD5�빤�����ɲ�һ�������Ǳ�1991����Ǹ��ϵ�
MD5�����ɵĿ�

string test = "a";
char md5Buf[33]={0};
md5Buf[33]='\0';
bool bRet = GenerateMemInfo(test.c_str(), test.length(), md5Buf);

string filename = "E:\\game\\9yin1.exe";
char md5FileBuf[32]={"\0"};
bool bRet = GetMD5FromFile(filename.c_str(), md5FileBuf);
*/
/************************************************************************/


// �����ڴ���MD5��
bool GenerateMemInfo( const char* lpMem, unsigned long dwSize, char bufMD5[32] );

//���ļ�����md5��
bool GetMD5FromFile(const char* szFileName, char bufMD5[32]);

#endif//LQINC_MD5LQUTILS_H
