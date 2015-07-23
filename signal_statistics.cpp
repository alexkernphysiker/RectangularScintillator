// this file is distributed under 
// GPL v 3.0 license
#include "signal_statistics.h"
#include "rectscinexception.h"
using namespace std;
SignalStatictics::SignalStatictics(){}
SignalStatictics::~SignalStatictics(){}
void SignalStatictics::AcceptEventStart(){}
void SignalStatictics::AcceptSignalValue(double time){AddValue(time);}
void SignalStatictics::AcceptEventEnd(){}

SignalDistribution::SignalDistribution(double from, double to, int bincount):m_data(from,to,bincount){
	f=from;t=to;cnt=bincount;
}
SignalDistribution::~SignalDistribution(){}
void SignalDistribution::PlotAndClear(string name){
	m_plotter.HistWithStdError(name,static_right(m_data));
	m_data=Distribution<double>(f,t,cnt);
}
void SignalDistribution::AcceptEventStart(){}
void SignalDistribution::AcceptSignalValue(double time){m_data.AddValue(time);}
void SignalDistribution::AcceptEventEnd(){}

Signal2DCorrelation::Signal2DCorrelation(){}
Signal2DCorrelation::~Signal2DCorrelation(){}
void Signal2DCorrelation::Start(){}
void Signal2DCorrelation::Process(Vec&& signals){
	if(signals.size()!=2)
		throw RectScinException("Signal2DCorrelation: error ");
	m_data.push_back(make_pair(signals[0],signals[1]));
}
void Signal2DCorrelation::Finish(){}
void Signal2DCorrelation::PlotAndClear(string name){
	m_plotter.WithoutErrors(name,static_right(m_data));
	m_data.clear();
}
