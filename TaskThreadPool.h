#ifndef TASK_THREAD_POOL_H
#define TASK_THREAD_POOL_H

#include <QObject>
#include <QThread>
#include <QVector>
#include <QList>
#include <QMap>
#include <QMutex>
#include "ThreadTask.h"
#include "ThreadObject.h"


class TaskThreadPool: public QObject
{
	Q_OBJECT
public:
	TaskThreadPool(QObject *parent,ThreadTask * pTaskItem,int nThreads = 2);
	~TaskThreadPool();

protected:
	QVector<QThread *> ThreadPool_;
	QVector<ThreadObject *> ThreadObjs_;
	ThreadTask * pThreadTaskItem_;
	int nThreads_;

//隐约感觉这个线程池很强大！
private:
	//各个m_ThreadPool\m_ThreadObjs的任务数
	QMap<QObject *,qint32> map_Tasks_;	   

	//各个data_source 目前的处理线程
	QMap<QObject *,QObject *> map_busy_source_task_;	
	//各个data_source 目前的排队数目
	QMap<QObject *,int> map_busy_source_counter_;	    

public slots:
	void append_new(QObject * data_source, const QByteArray & data);

	//捕获::sig_process_finished, 以便管理data_source的 FIFO 顺序
	void on_sig_process_finished(QObject * data_source);
signals:
	//************************************
	// Method:    do_task
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: QObject *		任务来源 （相同任务源的任务，在队列非空时会被安排到同一个线程处理，以确保对相同源的FIFO）
	// Parameter: QByteArray	任务体	
	// Parameter: QObject *		处理任务的线程对象（QGHThreadObject）
	//************************************
	void do_task(QObject *, const QByteArray &,QObject *);
	void done_task(QObject * data_source);
};


#endif


