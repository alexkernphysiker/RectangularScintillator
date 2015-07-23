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

SignalDistribution::SignalDistribution(string name, double from, double to, int bincount):
	Distribution<double,double>(from,to,bincount){
	m_name=name;
}
SignalDistribution::~SignalDistribution(){
	m_plotter.HistWithStdError(m_name,static_cast<Distribution<double>&&>(*this));
}
void SignalDistribution::AcceptEventStart(){}
void SignalDistribution::AcceptSignalValue(double time){AddValue(time);}
void SignalDistribution::AcceptEventEnd(){}

Signal2DCorrelation::Signal2DCorrelation(string name){m_name=name;}
Signal2DCorrelation::~Signal2DCorrelation(){
	m_plotter.WithoutErrors(m_name,static_right(m_data));
}
void Signal2DCorrelation::Start(){}
void Signal2DCorrelation::Process(Vec&& signals){
	if(signals.size()!=2)
		throw RectScinException("Signal2DCorrelation: error ");
	m_data.push_back(make_pair(signals[0],signals[1]));
}
void Signal2DCorrelation::Finish(){}
