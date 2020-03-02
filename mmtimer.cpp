#include "mmtimer.h"
#ifdef __MINGW32__ 
#define TIME_KILL_SYNCHRONOUS 0x0100
#endif

mmtimer::mmtimer(int interval, QObject *parent): 
	QObject(parent),m_interval(interval),m_id(0)
{
}

mmtimer::~mmtimer()
{
	 stop();
}

void mmtimer::start()
{
	m_id = timeSetEvent(m_interval, 1, mmtimer_proc, (DWORD_PTR)this,
		TIME_CALLBACK_FUNCTION | TIME_PERIODIC | TIME_KILL_SYNCHRONOUS);
}

void mmtimer::stop()
{
	if (m_id)
	{
		timeKillEvent(m_id);
		m_id = 0;
	}
}

void WINAPI CALLBACK mmtimer_proc(uint timerId, uint, DWORD_PTR user, DWORD_PTR, DWORD_PTR)
{
	mmtimer *t = reinterpret_cast<mmtimer *>(user);
	emit t->timeout();
}
