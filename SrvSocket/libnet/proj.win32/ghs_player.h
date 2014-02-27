#pragma once
class GHS_Player
{
public:
	GHS_Player(UI32 index,UI64 serial);
	virtual ~GHS_Player(void);
	inline UI64 getSerial()
	{
		return m_serial;
	}
private:
	UI32 m_index;
	UI64 m_serial;
};

