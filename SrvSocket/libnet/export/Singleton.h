#pragma once

template<typename T>
class Singleton
{
protected:
	static T*		m_instance;
public:
	static  T* shared();
};
template<typename T>
T* Singleton<T>::m_instance=0;

template<typename T>
T* Singleton<T>::shared()
{
	if(m_instance==0)
		m_instance=new T;
	return m_instance;
}