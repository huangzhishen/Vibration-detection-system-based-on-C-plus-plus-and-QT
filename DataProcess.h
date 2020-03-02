#ifndef DATA_PROCESS_H
#define DATA_PROCESS_H

#include "threadtask.h"
class DataProcess :public ThreadTask
{
public:
	DataProcess(QObject *parent);
	~DataProcess();
public:
	virtual void run(QObject * task_source, const QByteArray & data_array);
};

#endif

