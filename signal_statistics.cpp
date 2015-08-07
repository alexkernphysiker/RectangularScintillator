// this file is distributed under 
// GPL v 3.0 license
#include "signal_statistics.h"
#include "rectscinexception.h"
using namespace std;
SignalStatictics::SignalStatictics(){}
SignalStatictics::~SignalStatictics(){}
void SignalStatictics::AcceptEventStart(){}
void SignalStatictics::AcceptSignalValue(double time){m_data.AddValue(time);}
void SignalStatictics::AcceptEventEnd(){}
void SignalStatictics::Clear(){m_data=Sigma<double>();}
Sigma< double >&& SignalStatictics::data(){return static_right(m_data);}

SignalDistribution::SignalDistribution(double from, double to, int bincount):
	m_data(from,to,bincount){
	f=from;t=to;cnt=bincount;
}
SignalDistribution::~SignalDistribution(){}
void SignalDistribution::AcceptEventStart(){}
void SignalDistribution::AcceptSignalValue(double time){m_data.AddValue(time);}
void SignalDistribution::AcceptEventEnd(){}
void SignalDistribution::Clear(){m_data=Distribution<double>(f,t,cnt);}
Distribution< double >&& SignalDistribution::data(){return static_right(m_data);}


Signal2DCorrelation::Signal2DCorrelation(){}
Signal2DCorrelation::~Signal2DCorrelation(){}
void Signal2DCorrelation::Clear(){m_data.clear();}
vector< Pair >&& Signal2DCorrelation::Points(){
	return static_right(m_data);
}
void Signal2DCorrelation::Start(){}
void Signal2DCorrelation::Process(Vec&& signals){
	if(signals.size()!=2)
		throw RectScinException("Signal2DCorrelation: error ");
	if((isfinite(signals[0]))&&(isfinite(signals[1])))
	m_data.push_back(make_pair(signals[0],signals[1]));
}
void Signal2DCorrelation::Finish(){}

SignalsToFile::SignalsToFile(){}
SignalsToFile::~SignalsToFile(){
	if(file.is_open())file.close();
}
void SignalsToFile::Start(){}
void SignalsToFile::Process(Vec&& signals){
	if(file.is_open()){
		bool atleastone=false;
		for(double signal:signals)
			atleastone|=isfinite(signal);
		if(atleastone){
			for(double signal:signals)
				file<<signal<<" ";
			file<<"\n";
		}
	}
}
void SignalsToFile::Finish(){}
void SignalsToFile::Redirect(string name){
	if(file.is_open())file.close();
	file.open(name.c_str());
	if(!file.is_open())
		throw RectScinException("cannot open output file");
}