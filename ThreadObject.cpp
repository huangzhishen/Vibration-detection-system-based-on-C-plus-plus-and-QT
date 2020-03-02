#include "ThreadObject.h"
#include <assert.h>
#include <QDebug>
#include <QThread>

//�������ײ���ThreadTask������
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

//����ۺ�����˭����ѽ��
//ͬʱ������һ���ź�sig_process_finished��˭�յ�ѽ��
void ThreadObject::process(QObject * data_source, const QByteArray &data, QObject * target)
{
	if (target == this)
	{
		//qDebug() << "ThreadObject::process " <<  (quintptr)QThread::currentThreadId() << "data size" << data.size();
		array_.append(data);
		if (array_.size() >= 20000)//�ȵ�20000�����ݲŷ����źţ�ŶŶ��666����Ϊһ���źŵ�ռ�����ֽڣ�����10000�������20000��size��
		{                          //���ţ��߳������谭һ���ٷ�������ɵ��źţ�Ȼ���ڽ�����������ֻ�ǽ���5000���㣬����2�Ρ�
			QByteArray d(array_.left(20000));
			//qDebug() <<  (quintptr)QThread::currentThreadId() << "data size" << d.size();
			pThreadTask_->run(data_source,d);//666�����ҵ��ؼ��Ĵ����ˣ�
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