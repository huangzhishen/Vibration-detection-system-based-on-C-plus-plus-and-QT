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

//��Լ�о�����̳߳غ�ǿ��
private:
	//����m_ThreadPool\m_ThreadObjs��������
	QMap<QObject *,qint32> map_Tasks_;	   

	//����data_source Ŀǰ�Ĵ����߳�
	QMap<QObject *,QObject *> map_busy_source_task_;	
	//����data_source Ŀǰ���Ŷ���Ŀ
	QMap<QObject *,int> map_busy_source_counter_;	    

public slots:
	void append_new(QObject * data_source, const QByteArray & data);

	//����::sig_process_finished, �Ա����data_source�� FIFO ˳��
	void on_sig_process_finished(QObject * data_source);
signals:
	//************************************
	// Method:    do_task
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: QObject *		������Դ ����ͬ����Դ�������ڶ��зǿ�ʱ�ᱻ���ŵ�ͬһ���̴߳�����ȷ������ͬԴ��FIFO��
	// Parameter: QByteArray	������	
	// Parameter: QObject *		����������̶߳���QGHThreadObject��
	//************************************
	void do_task(QObject *, const QByteArray &,QObject *);
	void done_task(QObject * data_source);
};


#endif


