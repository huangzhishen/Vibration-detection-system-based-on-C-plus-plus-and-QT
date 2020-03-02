#ifndef MMTIMER_H
#define MMTIMER_H
#include <QObject>
#include <windows.h>
#include <MMSystem.h>
#pragma comment(lib,"winmm.lib")

class mmtimer:public QObject
{
	Q_OBJECT
public:
	mmtimer(int interval, QObject *parent = 0);
	~mmtimer();
signals:
	void timeout();

	public slots:
		void start();
		void stop();
		friend void WINAPI CALLBACK mmtimer_proc(uint, uint, DWORD_PTR, DWORD_PTR, DWORD_PTR);

private:
	int m_interval;
	int m_id;
};

#endif


