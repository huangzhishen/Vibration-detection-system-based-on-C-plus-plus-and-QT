#ifndef THREAD_TASK_H
#define THREAD_TASK_H
#include <QObject>
class ThreadTask:public QObject
{
public:
	ThreadTask(QObject *parent){};
	~ThreadTask(void){};
public:
	virtual void run(QObject * task_source, const QByteArray & data_array) = 0;
};
#endif

