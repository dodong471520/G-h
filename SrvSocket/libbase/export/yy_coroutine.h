#ifndef _COROUTINE_H_20130204_
#define _COROUTINE_H_20130204_

#include "yy_macros.h"
#include "FastDelegate.h"
NS_YY_BEGIN

//http://www.chiark.greenend.org.uk/~sgtatham/coroutines.html

//���Ƕ����ã�һ������Level�������
//���ö�ʱ���������߳�ÿһ֡����һ��ע���Э�̵ĺ���
//�ѹ����̴߳�����Ϣʱ��һЩ��ʱ�Ĵ�����coroutine����װ(sleep)����֤�����̲߳�����


#define crBegin				static int state=0; switch(state) { case 0:
#define crReturn(x)			do { state=__LINE__; return x; case __LINE__:; } while (0)
#define crFinish			}



bool function(void)
 {
	static int n=10;
    crBegin;

    while(--n)
	{
		//printf("%d\n", n);
        crReturn(false);
	}

    crFinish;
	return true;
}
/*


	Coroutine test;
	test.create(function);

	while(true)
	{
		test.onTick();
		Sleep(1000);
	}

typedef fastdelegate::FastDelegate0<bool> CoroutineCallback;

class Coroutine
{
public:
	//run all coroutine event in every tick
	void onTick();

	void create(CoroutineCallback cb);
	void yield();
	void resume();

private:
	std::vector<int> coroutine_set_;
};*/


NS_YY_END
#endif