#include "TaskThreadPool.h"
#include <QDebug>

TaskThreadPool::TaskThreadPool(QObject *parent,ThreadTask * pTaskItem,int nThreads)
	:QObject(parent),nThreads_(nThreads),pThreadTaskItem_(pTaskItem)
{
	for (int i=0;i<nThreads;i++)
	{
		//优化 内存泄漏 securitysystem.cpp (187)
		QThread * pNewThread = new QThread(this);
		ThreadObject * pNewObject = new ThreadObject(0,pTaskItem);

		ThreadPool_.push_back(pNewThread);
		ThreadObjs_.push_back(pNewObject);
		map_Tasks_[pNewObject] = 0;

		pNewThread->start();
		pNewObject->moveToThread(pNewThread);

		//连接处理完成消息
		connect(pNewObject,SIGNAL(sig_process_finished(QObject *)),this,SLOT(on_sig_process_finished(QObject *)));
		//连接处理新任务消息
		connect(this,SIGNAL(do_task(QObject *, const QByteArray &,QObject *)),pNewObject,SLOT(process(QObject *, const QByteArray &,QObject *)));
	}
}


TaskThreadPool::~TaskThreadPool()
{
	foreach(ThreadObject * obj, ThreadObjs_)
	{
		disconnect(obj,SIGNAL(sig_process_finished(QObject *)),this,SLOT(on_sig_process_finished(QObject *)));
		disconnect(this,SIGNAL(do_task(QObject *, const QByteArray &,QObject *)),obj,SLOT(process(QObject *, const QByteArray &,QObject *)));
		obj->deleteLater();
	}
	foreach(QThread * th ,ThreadPool_)
	{
		th->exit(0);//这个学过，进程结束，等待结束
		th->wait();
	}
}



void TaskThreadPool::append_new(QObject * data_source, const QByteArray & data)
{
	//qDebug() << QString("client %2 Recieved %3 bytes.").arg((quintptr)data_source).arg(data.size());
	//qDebug() << "TaskThreadPool::append_new " <<  (quintptr)QThread::currentThreadId();
	QObject * pMinObj = 0;
	//对一批来自同一数据源的数据，使用同样的数据源处理，以免发生多线程扰乱FIFO对单个data_source的完整性
	if (map_busy_source_counter_.find(data_source) != map_busy_source_counter_.end())//这个判断很好！
	{
		map_busy_source_counter_[data_source]++;
		pMinObj = map_busy_source_task_[data_source];
	}
	else
	{
		qint32 nMinCost = 0x7fffffff;
		//寻找现在最空闲的一个线程
		for (QMap<QObject *,qint32>::iterator p = map_Tasks_.begin(); p!=map_Tasks_.end() ; p++)
		{
			if (p.value() < nMinCost)//666这个是找最小值，设为最大，迭代替换，得最小值。
			{
				nMinCost = p.value();
				pMinObj = p.key();
			}
		}
		if (pMinObj)
		{
			map_busy_source_counter_[data_source] = 1;
			map_busy_source_task_[data_source] = pMinObj;
		}
	}
	if (pMinObj)
	{
		map_Tasks_[pMinObj]++;
		emit do_task(data_source,data,pMinObj);
	}

}

void TaskThreadPool::on_sig_process_finished(QObject * data_source)
{
	if (map_Tasks_.find(sender()) != map_Tasks_.end())
	{
		map_Tasks_[sender()]--;//该线程的任务数减一
	}

	if (map_busy_source_counter_.find(data_source) != map_busy_source_counter_.end())
	{
		map_busy_source_counter_[data_source]--;//该数据源（sock）的任务数减一
		if (map_busy_source_counter_[data_source]<=0)
		{
			map_busy_source_counter_.remove(data_source);//如果为0，则在map中remove
			map_busy_source_task_.remove(data_source);
		}
	}
	emit done_task(data_source);
}
