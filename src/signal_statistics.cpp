// this file is distributed under 
// MIT license
#include <math_h/error.h>
#include <RectScin/signal_statistics.h>
namespace RectangularScintillator{
	using namespace std;
	using namespace MathTemplates;
	SignalStatictics::SignalStatictics(){}
	SignalStatictics::~SignalStatictics(){}
	void SignalStatictics::AcceptEventStart(){}
	void SignalStatictics::AcceptSignalValue(double time){m_data<<time;}
	void SignalStatictics::AcceptEventEnd(){}
	const Sigma<double>&SignalStatictics::data()const{return m_data;}
	
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
			throw Exception<SignalsToFile>("cannot open output file");
	}
	
	SignalAnalyse::SignalAnalyse(function<void(double)> f){func=f;}
	SignalAnalyse::~SignalAnalyse(){}
	void SignalAnalyse::AcceptEventStart(){}
	void SignalAnalyse::AcceptSignalValue(double time){func(time);}
	void SignalAnalyse::AcceptEventEnd(){}
	SignalsAnalyse::SignalsAnalyse(function<void(const Vec&)> f){fnc=f;}
	SignalsAnalyse::~SignalsAnalyse(){}
	void SignalsAnalyse::Start(){}
	void SignalsAnalyse::Process(const Vec& signals){fnc(signals);}
	void SignalsAnalyse::Finish(){}
};