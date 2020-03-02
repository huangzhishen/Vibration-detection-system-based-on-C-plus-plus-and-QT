#ifndef SHOWDATA_H
#define SHOWDATA_H
#include <QDialog>
#include <qwt_plot.h>  
#include <qwt_plot_layout.h>  
#include <qwt_plot_canvas.h>  
#include <qwt_plot_renderer.h>  
#include <qwt_plot_grid.h>  
#include <qwt_plot_histogram.h>  
#include <qwt_plot_curve.h>  
#include <qwt_plot_zoomer.h>  
#include <qwt_plot_panner.h>  
#include <qwt_plot_magnifier.h>  

#include <qwt_legend.h>  
#include <qwt_legend_label.h>  
#include <qwt_column_symbol.h>  
#include <qwt_series_data.h>  
#include <qpen.h>  
#include <qwt_symbol.h>  
#include <qwt_picker_machine.h>  
#include "ui_dataShow.h"
class ShowData:public QDialog
{
	Q_OBJECT
public:
	ShowData(float data[],int size,QDialog *parent = 0);
	~ShowData();

private slots:
	void on_btnSignal_clicked();
private:
	Ui::showData ui_;
	float *data_;
	QwtPlotCanvas *canvas_;
	QwtPlotGrid *grid_;
	QwtPlotCurve *curve_;
	QwtSymbol *symbol_;
	QwtPlotMagnifier *magifier_;
	QwtPlotPanner *panner_;
	QwtPlotZoomer *zoomer_;
	int size_;
	bool isOrigin_;

	void setData(float data[],int size);
};

#endif

