#include "ThreadObject.h"
#include <assert.h>
#include <QDebug>
#include <QThread>

//终于明白参数ThreadTask的作用
ThreadObject::ThreadObject(QObject *parent,ThreadTask * pThreadTask)
:QObject(parent),pThreadTask_(pThreadTask)
{
	assert(pThreadTask!=NULL);
	array_.clear();
	array_.reserve(20000);
}

ThreadObject::~ThreadObject(void)
{
}        

//这个槽函数，谁调用呀！
//同时发送了一个信号sig_process_finished，谁收的呀！
void ThreadObject::process(QObject * data_source, const QByteArray &data, QObject * target)
{
	if (target == this)
	{
		//qDebug() << "ThreadObject::process " <<  (quintptr)QThread::currentThreadId() << "data size" << data.size();
		array_.append(data);
		if (array_.size() >= 20000)//等到20000个数据才发送信号，哦哦，666，因为一个信号点占两个字节，所以10000个点就是20000的size了
		{                          //嗯嗯，线程在这阻碍一下再发处理完成的信号，然后在接受数据那里只是接受5000个点，接受2次。
			QByteArray d(array_.left(20000));
			//qDebug() <<  (quintptr)QThread::currentThreadId() << "data size" << d.size();
			pThreadTask_->run(data_source,d);//666终于找到关键的代码了！
			array_.clear();
			array_.reserve(20000);
			emit sig_process_finished(data_source);
		}
	}
}

void ThreadObject::SetThreadTaskItem(ThreadTask * pThreadTask)
{
	assert(pThreadTask!=NULL);
	pThreadTask_ = pThreadTask;
}