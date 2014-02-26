#ifndef _SharedMemORY_H_20130204_
#define _SharedMemORY_H_20130204_


#include "yy_macros.h"

NS_YY_BEGIN

/************************************************************************/
/* 
@class:    ShareMem
@brief:    �����ڴ�

ShareMem<ShareMemoryTest> g_smdt("test");
g_smdt->index = num;
g_smdt->msg = strMsg;
*/
/************************************************************************/
template<class ShareMemoryDataType>
class ShareMem
{
public:
	ShareMem();
	ShareMem(const char* name);
	~ShareMem();

	void createShareMemory(const char* name);

	//ע����
	ShareMemoryDataType *operator->();
	ShareMemoryDataType* getData();
	void clear();

private:
	char	shared_name_[MAXSHAREMEMLEN];			//�����ڴ��ʶ��
	unsigned long	max_data_size;					//�����ڴ�����ֵ
	ShareMemoryDataType*	data_;					//�����ڴ��д�ŵ�����
	bool	created_;								//�����ڴ��Ƿ��Ѿ�����
	HANDLE	file_mapping_;							//�����ڴ��ļ��ľ��
};



template<class ShareMemoryDataType>
ShareMem<ShareMemoryDataType>::ShareMem()
{
	data_ = NULL;
	file_mapping_ = NULL;
}


template<class ShareMemoryDataType>
ShareMem<ShareMemoryDataType>::ShareMem(const char* name)
{
	ShareMem();
	createShareMemory(name);
}

template<class ShareMemoryDataType>
ShareMem<ShareMemoryDataType>::~ShareMem()
{
	if(data_)
	{
		//��ӳ�����ӵ�ǰӦ�ó���ĵ�ַ�ռ�unmap
		UnmapViewOfFile(data_);
		data_ = NULL;
	}

	if(file_mapping_)
	{
		CloseHandle(file_mapping_);
	}
}

template<class ShareMemoryDataType>
ShareMemoryDataType *ShareMem<ShareMemoryDataType>::operator->()
{
	return data_;
}

template<class ShareMemoryDataType>
ShareMemoryDataType* ShareMem<ShareMemoryDataType>::getData()
{
	return data_;
}

template<class ShareMemoryDataType>
void ShareMem<ShareMemoryDataType>::clear()
{
	memset(data_, 0, max_data_size); 
}

template<class ShareMemoryDataType>
void ShareMem<ShareMemoryDataType>::createShareMemory(const char* name)
{
	max_data_size = sizeof(ShareMemoryDataType);

	strcpy_s(shared_name_, sizeof(shared_name_), name);

	//�����ļ�ӳ���ں˶���
	file_mapping_ = CreateFileMappingA(INVALID_HANDLE_VALUE,
		NULL,							// Ĭ�ϵİ�ȫ����
		PAGE_READWRITE,					// �ɶ�д�ļ�������
		0,								// size: high 32-bits������4GB���µ��ļ����ò�������0
		sizeof(ShareMemoryDataType),	// size: low 32-bits��ӳ���ļ����ֽ���
		name);							// ������ӳ��������ں��������̹���ö���

	if(file_mapping_ == NULL)
	{
		int err = GetLastError();
		THROWNEXCEPT("CreateFileMappingA error no:%d", err);
	}

	//�ж��Ƿ��һ�����ʹ����ڴ�ĳ���
	created_ = (GetLastError() == ERROR_ALREADY_EXISTS); 

	//// ���ù����ڴ�ķ���Ȩ��
	//SetNamedSecurityInfo(name,
	//	SE_KERNEL_OBJECT,
	//	DACL_SECURITY_INFORMATION,	//// change only the object's DACL
	//	0,	// do not change owner or group
	//	0,	// do not change owner or group
	//	(PACL)NULL,	//no   DACL   specified   -   allows   everyone   access 
	//	NULL);	// do not change SACLϵͳ���ʿ����б�


	//��һ���ļ�ӳ�����ӳ�䵽��ǰӦ�ó���ĵ�ַ�ռ�
	data_ = (ShareMemoryDataType*)MapViewOfFile(file_mapping_,
		FILE_MAP_READ | FILE_MAP_WRITE,		//���Զ�д�ļ�
		0,	// high offset:  map from
		0,	// low offset:   beginning
		0);	 // default: map entire file

	if(data_ == NULL)
	{
		CloseHandle(file_mapping_);
		THROWNEXCEPT("MapViewOfFile error.");
	}

	//����ǵ�һ�����������ڴ�ĳ������ʼ��
	if (!created_) 
	{
		clear();
	}
}


NS_YY_END
#endif