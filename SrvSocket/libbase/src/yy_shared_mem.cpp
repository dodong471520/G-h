#include "yy_shared_mem.h"
#include "yy_exception.h"
#include <Windows.h>
#define MAXSharedMemLEN 100

NS_YY_BEGIN

class SharedMemImpl
{
public:
	SharedMemImpl(const char* name, size_t size);
	~SharedMemImpl();
	void* getData();

private:
	void createSM(const char* name, size_t size);

private:
	void* data_;
	char	shared_name_[MAXSharedMemLEN];			//�����ڴ��ʶ��
	unsigned long	max_data_size;					//�����ڴ�����ֵ
	bool	created_;								//�����ڴ��Ƿ��Ѿ�����
	HANDLE	file_mapping_;							//�����ڴ��ļ��ľ��
};


SharedMemImpl::SharedMemImpl(const char* name, size_t size)
{
	createSM(name, size);
}

SharedMemImpl::~SharedMemImpl()
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

void* SharedMemImpl::getData()
{
	return data_;
}

void SharedMemImpl::createSM(const char* name, size_t size)
{
	strcpy_s(shared_name_, sizeof(shared_name_), name);

	//�����ļ�ӳ���ں˶���
	file_mapping_ = CreateFileMappingA(INVALID_HANDLE_VALUE,
		NULL,							// Ĭ�ϵİ�ȫ����
		PAGE_READWRITE,					// �ɶ�д�ļ�������
		0,								// size: high 32-bits������4GB���µ��ļ����ò�������0
		size,							// size: low 32-bits��ӳ���ļ����ֽ���
		name);							// ������ӳ��������ں��������̹���ö���

	if(!file_mapping_)
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
	data_ = MapViewOfFile(file_mapping_,
		FILE_MAP_READ | FILE_MAP_WRITE,		//���Զ�д�ļ�
		0,	// high offset:  map from
		0,	// low offset:   beginning
		0);	 // default: map entire file

	if(!data_)
	{
		CloseHandle(file_mapping_);
		THROWNEXCEPT("MapViewOfFile error.");
	}

	//����ǵ�һ�����������ڴ�ĳ������ʼ��
	if (!created_) 
	{
		memset(data_, 0, size);
	}
}


NS_YY_END