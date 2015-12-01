// this file is distributed under 
// MIT license
#include <exception_math_h.h>
#include <signal_statistics.h>
using namespace std;
SignalStatictics::SignalStatictics(){}
SignalStatictics::~SignalStatictics(){}
void SignalStatictics::AcceptEventStart(){}
void SignalStatictics::AcceptSignalValue(double time){m_data.AddValue(time);}
void SignalStatictics::AcceptEventEnd(){}
void SignalStatictics::Clear(){m_data=Sigma<double>();}
Sigma<double>&SignalStatictics::data()const{return const_cast<Sigma<double>&>(m_data);}

SignalDistribution::SignalDistribution(double from, double to, int bincount):
	m_data(from,to,bincount){
	f=from;t=to;cnt=bincount;
}
SignalDistribution::~SignalDistribution(){}
void SignalDistribution::AcceptEventStart(){}
void SignalDistribution::AcceptSignalValue(double time){m_data.AddValue(time);}
void SignalDistribution::AcceptEventEnd(){}
void SignalDistribution::Clear(){m_data=Distribution<double>(f,t,cnt);}
Distribution<double>&SignalDistribution::data()const{return const_cast<Distribution<double>&>(m_data);}

SignalsToFile::SignalsToFile(){}
SignalsToFile::~SignalsToFile(){
	if(file.is_open())file.close();
}
void SignalsToFile::Start(){}
void SignalsToFile::Process(const Vec&signals){
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
		throw math_h_error<SignalsToFile>("cannot open output file");
}