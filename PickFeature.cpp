#include "PickFeature.h"
#include "haar_classic.h"
#include "packcontainer.h"
#include "packtree.h"
#include <qDebug>
#include <opencv2/opencv.hpp>
using namespace cv;  

void diffData(const vector<float> &data,vector<float> &dData);
float getMaxElement(vector<float> &data);
int getPeakCount(vector<float> &data, float gap,vector<float> &vecPeaks);
int getOverThreshold(vector<float> &data,float threshold);
void calulateEnergy(vector<float> &data, int n, vector<float> &energy);
float getOverEnergy(vector<float> &data);
void getShave(vector<float> &data, int n, vector<float> &midData);
void getWavePackage(const vector<float> &data,vector<float> &wavePackage);
void enframe(vector<float> &data, int wlen, int mlen,vector<vector<float>> &f);
void vad(vector<float> &data,int &start, int &stop, vector<float> &zcr, vector<float> &amp);


void PickFeature::preProcess(vector<float> &data)
{
	//均值
	float sum = 0.0;
	for (int i = 0 ; i < data.size() ; i++)
	{
		sum += data[i];
	}
	float mean =  sum / data.size();

	float accum  = 0.0;  
	std::for_each (std::begin(data), std::end(data), [&](const float d) {  
		accum  += (d-mean)*(d-mean);  
	});  
	float var = accum/ data.size(); //方差
	float stdvar = sqrt(var);

	for (int i = 1; i < data.size() ;i++)//修正
	{
		if (abs(data[i]-mean) > 3 * stdvar)
		{
			data[i] = data[i-1];
		}
	}
}
void PickFeature::smoothWave(vector<float> &dData)//平滑，疑问？
{
	for (int i = 0 ; i < dData.size()-1 ; i++)
	{
		if ( (i+1) % 50 == 0 )
		{
			dData[i] = dData[i-1];
		}
	}
}
void PickFeature::getFeature(const vector<float> &data, vector<float> &feature)
{
	vector<float> dData;
	diffData(data,dData);//取查分信号
	feature.push_back(getMaxElement(dData));

	vector<float> vecPeaks;
	vecPeaks.reserve(1000);//预留1000个数据
	feature.push_back(getPeakCount(dData, 0.005, vecPeaks));
	feature.push_back(getMaxElement(vecPeaks));
	int ret;
	feature.push_back(getOverThreshold(dData,0.01));
	feature.push_back(getOverEnergy(dData));

	vector<float> midData;
	midData.reserve(1000);
	getShave(dData,100,midData);
	double sum = 0.0;  
	for (int i = 0 ; i < midData.size() ; i++)
	{
		sum += midData[i];
	}
	double mean =  sum / midData.size(); //均值  

	double accum  = 0.0;  
	std::for_each (std::begin(midData), std::end(midData), [&](const double d) {  
		accum  += (d-mean)*(d-mean);  
	});  

	double stdev = sqrt(accum/(midData.size()-1)); //方差 
	double value = stdev / mean;
	feature.push_back(value	);
	feature.push_back(mean);

//	vector<float> waveData;
//	getWavePackage(dData,waveData);
//	feature.insert(feature.end(),waveData.begin(),waveData.end());
}

void PickFeature::getZCRandAmp(vector<float> &data,float &zcr, float &amp)
{
	preProcess(data);
	vector<float> dData;
	diffData(data,dData);
	//smoothWave(dData);
	int start;
	int stop;
	vector<float> vZCR;
	vector<float> vAmp;
	vad(dData,start,stop,vZCR,vAmp);
	float sumZCR = 0.0;
	float sumAmp = 0.0;
	for (int i = start; i <= stop ; i++)
	{
		sumZCR += vZCR[i];
		sumAmp += vAmp[i];
	}
	zcr = sumZCR/(stop-start);
	amp	= sumAmp/(stop-start);
}

//差分信号
void diffData(const vector<float> &data,vector<float> &dData)
{
	dData.reserve(data.size()-1);
	for (int i = 1 ; i < data.size(); i++)
	{
		dData.push_back(data[i] - data[i-1]);
	}
}
//取最大值
float getMaxElement(vector<float> &data)
{
	float maxElem = 0.0;
	for(int i = 1 ; i < data.size() ; i++){
		maxElem = max(maxElem,data[i]);
	}
	return maxElem;
}

//？
int getPeakCount(vector<float> &data, float gap,vector<float> &vecPeaks){
	if (data.size() <= 1)
	{
		return 0;
	}
	int peakCount = 0;
	float maxElem = data[0];
	float minElem = data[0];
	bool flag = data[1] > data[0];
	for (int i = 1 ; i < data.size(); i++)
	{
		if (flag == true)
		{
			if (data[i] < data[i-1] && abs(data[i] - data[i-1]) > gap)
			{
				peakCount++;
				maxElem = data[i-1];
				vecPeaks.push_back(maxElem - minElem);
				flag = !flag;
			}
		}
		else
		{
			if (data[i] > data[i-1] && abs(data[i] - data[i-1]) > gap)
			{
				peakCount++;
				minElem = data[i-1];
				vecPeaks.push_back(minElem - maxElem);
				flag = !flag;
			}
		}
	}
	return peakCount;
}

int getOverThreshold(vector<float> &data,float threshold)
{
	int count = 0;
	for (int i = 0 ; i < data.size(); i++)
	{
		if (abs(data[i]) > threshold)
		{
			count++;
		}
	}
	return count;
}

void calulateEnergy(vector<float> &data, int n, vector<float> &energy)
{
	int num = 1;
	float sum = 0.0;
	for (int i = 0 ; i < data.size() ; i++)
	{
		sum += data[i] * data[i];
		if ( num % n == 0)
		{
			energy.push_back(sum);
			sum = 0.0;
		}
		num ++;
	}
}

template <typename T>
int sign (const T &val) 
{ 
	return (val > 0) - (val < 0);
}

float getOverEnergy(vector<float> &data){
	float num = 0;
	vector<float> energy;
	energy.reserve(1000);
	calulateEnergy(data,10,energy);
	float y = getMaxElement(energy);
	y = 0.0002+0.1*y;

	for (int i = 1 ; i < energy.size(); i++)
	{
		num = num + 0.5 * abs (sign(energy[i]-y)-sign(energy[i-1]-y) ) ;
	}
	return num;
}

void getShave(vector<float> &data, int n, vector<float> &midData){
	for (int i = n ; i < data.size(); i+= n)
	{
		midData.push_back(getMaxElement(vector<float>(data.begin()+i-n,data.begin()+i)));
	}
}

void getWavePackage(const vector<float> &data,vector<float> &wavePackage)
{
	haar_classic<packcontainer> h;
	packtree tree( data.data(), data.size(), &h );
	//qDebug() << "before svd.compute";
	vector<vector<float>> res = tree.getLevel3Data();
	
	for (int i = 0 ; i < res.size() ; i++)
	{
		Mat U, W, VT;
		SVD svd;  
		Mat waveData(1,res[i].size() , CV_32FC1,res[i].data());
		
		svd.compute(waveData, W, U, VT); 
		
		wavePackage.push_back(W.at<float>(0));
	}
	//qDebug() << "after svd.compute";
}


void enframe(vector<float> &data, int wlen, int mlen,vector<vector<float>> &f)
{
	int datalen = data.size();
	int nf = (datalen - wlen + mlen)/mlen;//计算帧数
	f.clear();
	f.resize(nf,vector<float>(wlen,0));//初始化
	for (int i = 0 ; i < nf; i++)
	{
		for (int j = 0 ; j < wlen ; j++)
		{
			f[i][j] = data[i*mlen+j];
		}
	}
}

void vad(vector<float> &data,int &start, int &stop, vector<float> &zcr, vector<float> &amp)//ljkkj
{
	int framelen = 200;
	int moveLen = 100;

	float amp1 = 10;
	float amp2 = 2;
	int zcr1 = 10;
	int zcr2 = 2;

	vector<vector<float>> tmp1;
	vector<vector<float>> tmp2;
	vector<float> d1(data.begin(), data.end()-1);//复制数据
	vector<float> d2(data.begin()+1,data.end());

	enframe(d1,framelen,moveLen,tmp1);
	enframe(d2,framelen,moveLen,tmp2);
	zcr.clear();
	for (int i = 0 ; i <tmp1.size(); i++)
	{
		float s = 0.0;
		for (int j = 0 ; j < tmp1[0].size();j++)
		{
			int x1 = (tmp1[i][j] * tmp2[i][j] < 0);
			int x2 = abs(tmp1[i][j]- tmp2[i][j]) > 0.09;
			s += x1 * x2;
		}
		zcr.push_back(s);
	}
	vector<vector<float>> f;
	enframe(data,framelen,moveLen,f);
	amp.clear();
	float maxEnergy = 0.0;
	for (int i = 0 ; i < f.size(); i++)
	{
		float s = 0.0;
		for (int j = 0; j < f[0].size(); j++)
		{
			s += pow(abs(f[i][j]),2);//可以加平方
		}
		amp.push_back(s);
		maxEnergy = max(maxEnergy,s);
	}
	amp1 = min(amp1,maxEnergy/4);//////////////////////////////////////////////////////////////////////////
	amp2 = min(amp2,maxEnergy/8);//////////////////////////////////////////////////////////////////////////

	int maxSilence = 8;  //语音段中的静音帧数未超过此值，则认为语音还没结束
	int minLen = 15;  //若语音段长度小于此值，则认为其为一段噪音
	int status = 0;
	int count = 0;//语音段
	int silence = 0;//静音段
	start = 0;
	stop = 0;

	for (int i = 0 ; i < zcr.size(); i++)
	{
		switch (status)
		{
		case 0:
			if (amp[i] > amp1)//danger
			{
				start = max(i-count-1,0);
				status =2;
				silence = 0;
				count++;
			}
			else if (amp[i] > amp2)
			{
				status	= 1;
				count++;
			}
			else{
				status = 0;
				count = 0;
			}
			break;
		case 1:
			if (amp[i] > amp1)//danger
			{
				start = max(i-count-1,0);
				status =2;
				silence = 0;
				count++;
			}
			else if (amp[i] > amp2)
			{
				status	= 1;
				count++;
			}
			else{
				status = 0;
				count = 0;
			}
			break;
		case 2:
			if (amp[i] > amp2)
			{
				count++;
			}
			else
			{
				silence++;
				if (silence < maxSilence)
				{
					count++;
				}
				else if (count < minLen)//噪音，舍去
				{
					status  = 0;
					silence = 0;
					count = 0;
				}
				else
					status = 3;
			}
			break;
		case 3:
			break;		
		}
	}
	count = count-silence/2;
	stop = start+count-1;
}