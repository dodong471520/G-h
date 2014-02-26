/*
@author:  junliang
@brief:   freelist
@time:    20130327
*/
#ifndef _FREELIST_H_
#define _FREELIST_H_

#include "yy_macros.h"
#include "yy_memory.h"
#include "yy_synchronize.h"
NS_YY_BEGIN

//��new/delete, ������malloc/free, �ô���TYPE�ṹ����԰����࣬���Զ���ʼ��
template<typename TYPE>
class FreeList
{
private:
	//������ʽ����
	struct Node
	{
		TYPE data_;		//dataһ��Ҫ�ڵ�һ����������free��ʱ��ǿתokay
		Node * next_node_;
	};

public:
	FreeList();
	~FreeList();

	//�����µ�elem
	TYPE * allocElem();

	//�ͷ�elem
	void freeElem(TYPE * p);

	//void reserve();Ԥ����
private:
	void clear();
private:
	Node *	free_list_;		//��������
	CSLock	lock_;			//��
};



template<typename T>
FreeList<T>::FreeList()
{
	free_list_ = NULL;
}

template<typename T>
FreeList<T>::~FreeList()
{
	clear();
}


template<typename T>
T * FreeList<T>::allocElem()
{
	//return new TYPE();
	if (free_list_)
	{
		AutoLock lock(&lock_);
		if (free_list_)
		{
			Node* p = free_list_;
			free_list_ = p->next_node_;
			return &p->data_;
		}
	}

	Node * p=new Node;
	p->next_node_ = NULL;
	return &p->data_;
}

template<typename T>
void FreeList<T>::freeElem(T * pData)
{
	if(NULL == pData)
		return;

	//delete pData;
	AutoLock autolock(&lock_);
	Node* p = (Node *)pData;
	p->next_node_ = free_list_;
	free_list_ = p;

}


template<typename T>
void FreeList<T>::clear()
{
	AutoLock autolock(&lock_);

	Node * p;
	Node * t;
	p = free_list_;
	while (p)
	{
		t = p;
		p = p->next_node_;
		delete p;
	}

	free_list_ = NULL;
}


NS_YY_END
#endif