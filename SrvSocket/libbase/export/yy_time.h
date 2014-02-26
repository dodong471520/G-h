#ifndef _TIME_H_
#define _TIME_H_

#include "yy_macros.h"
#include "FastDelegate.h"
#include <string>

NS_YY_BEGIN

//get micro seconds from Epoch
UI64 TimeStamp();

//get current formatted time
std::string StrTimeStamp();


class Timer
{
private:
	typedef fastdelegate::FastDelegate0<bool> OnTimeCallback;
	struct TimerInfo
	{
		OnTimeCallback func;		//��ʱ�ص�����
		UI64 time_stamp;			//ʱ���
		UI32 time_interval;			//ʱ����,�뼶��
	};
public:
	Timer();
	~Timer();
	static Timer* getInstance();

	//register a timer, time interval����Ϊ��λ
	UI32 registerTimer(OnTimeCallback onTime, UI32 time_interval);
	void unRegisterTimer(UI32 timer_id);
	void run();
private:
	TimerInfo m_timers[10];
};



NS_YY_END

#endif