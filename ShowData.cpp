#include "ShowData.h"
#include "PickFeature.h"
#include <algorithm>
using namespace std;

ShowData::ShowData(float data[],int size,QDialog *parent)
	:QDialog(parent),canvas_(nullptr),grid_(nullptr),curve_(nullptr),magifier_(nullptr),panner_(nullptr),zoomer_(nullptr),symbol_(nullptr)
{
	ui_.setupUi(this);
	ui_.qwtPlot->setTitle(tr("���ź�"));
	data_ = new float[size];
	size_ = size;
	memcpy(data_,data,size * sizeof	(float));

	canvas_=new QwtPlotCanvas();  
	canvas_->setPalette(Qt::white);  
	canvas_->setBorderRadius(10);  
	ui_.qwtPlot->setAxisTitle( QwtPlot::yLeft, tr("��ѹ") );  
	ui_.qwtPlot->setAxisTitle( QwtPlot::xBottom, tr("ʱ��") );  
	ui_.qwtPlot->setAxisScale(QwtPlot::xBottom,0.0,2.0);  

	grid_ = new QwtPlotGrid;  
	grid_->enableX( true );//����������  
	grid_->enableY( true );  
	grid_->setMajorPen( Qt::black, 0, Qt::DotLine );  
	grid_->attach( ui_.qwtPlot );  

	curve_=new QwtPlotCurve("curve");  
	curve_->setPen(Qt::blue,1);//����������ɫ ��ϸ  
	curve_->setRenderHint(QwtPlotItem::RenderAntialiased,true);//�����⻬��  

	magifier_ = new QwtPlotMagnifier(ui_.qwtPlot->canvas());
	//���ƽ��
	panner_ = new QwtPlotPanner( ui_.qwtPlot->canvas());

	//ctrl + �����ʾ����ֵ�� �Ҽ��ָ�ԭ״
	zoomer_ = new QwtPlotZoomer( ui_.qwtPlot->canvas());    
	zoomer_->setMousePattern( QwtEventPattern::MouseSelect1, Qt::LeftButton, Qt::ControlModifier);

	symbol_ = new QwtSymbol( QwtSymbol::Ellipse,  
		QBrush( Qt::yellow ), QPen( Qt::red, 1 ), QSize( 1, 1) );//�������������ɫ����С  
	curve_->setSymbol( symbol_ );//�����������״  

	setData(data_,size_);
	isOrigin_ = true;

	vector<float> originData(data_,data_+size_);
	for (int i = 0;i < size_;i++)
	{
		originData[i] = originData[i]*5.0/4095;
	}
	float zcr;
	float amp;
	PickFeature::getZCRandAmp(originData,zcr,amp);
	
	ui_.leZcr->setText(QString("%1").arg(zcr));
	ui_.leAmp->setText(QString("%1").arg(amp));


}


ShowData::~ShowData(void)
{
	delete data_;
	
	if (canvas_)
	{
		delete canvas_;
	}
	if (grid_)
	{
		delete grid_;
	}
	if (curve_)
	{
		delete curve_;
	}
	
	if (magifier_)
	{
		delete magifier_;
	}
	if (panner_)
	{
		delete panner_;
	}
	if (zoomer_)
	{
		delete zoomer_;
	}
}

void ShowData::on_btnSignal_clicked()
{
	if (isOrigin_ == true)
	{
		ui_.btnSignal->setText(tr("ԭʼ�ź�"));
		vector<float> originData(data_,data_+size_);
		PickFeature::preProcess(originData);

		isOrigin_ = false;
		vector<float> diff(size_-1,0.0);
		for (int i = 1; i < size_;i++)
		{
			diff[i-1] = originData[i]-originData[i-1];
		}
		//PickFeature::smoothWave(diff);
		setData(diff.data(),size_-1);
	}
	else{
		ui_.btnSignal->setText(tr("����ź�"));
		isOrigin_ = true;
		setData(data_,size_);
	}
	
}

void ShowData::setData(float data[],int size)
{
	float maxValue = 0.0;
	float minValue = 4095.0;

	QPolygonF points1;//����ڵ�����QPointF(x,y)  
	for (int i = 0 ; i < size ; i++)
	{
		maxValue = max(maxValue,data[i]);
		minValue = min(minValue,data[i]);
		points1 << QPointF(0.0002*i,data[i]);
	}
	ui_.qwtPlot->setAxisScale(QwtPlot::yLeft,minValue-100,maxValue+100); 

	curve_->setSamples(points1);  
	curve_->attach( ui_.qwtPlot );
	ui_.qwtPlot->replot();
}