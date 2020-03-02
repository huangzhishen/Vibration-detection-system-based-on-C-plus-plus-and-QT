#ifndef THREAD_OBJECT_H
#define THREAD_OBJECT_H
#include <QObject>
#include <QByteArray>
#include "ThreadTask.h"
class ThreadObject:public QObject
{
		Q_OBJECT
public:
	ThreadObject(QObject *parent,ThreadTask * pThreadTask);
	~ThreadObject(void);
	void SetThreadTaskItem(ThreadTask * pThreadTask);

public slots:  
	void process(QObject * data_source, const QByteArray &data, QObject * target);

private:
	ThreadTask *pThreadTask_;
	QByteArray array_;

signals:
	void sig_process_finished(QObject * data_source);
};
#endif
