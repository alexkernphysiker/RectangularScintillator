// this file is distributed under 
// MIT license
#include <math_h/error.h>
#include <RectScin/photon2signal.h>
namespace RectangularScintillator{
	using namespace std;
	using namespace MathTemplates;
	OrderStatistics::OrderStatistics(size_t count){
		m_count=0;
		for(size_t i=0;i<count;i++)
			m_stat.push_back(Sigma<double>());
	}
	OrderStatistics::~OrderStatistics(){}
	size_t OrderStatistics::count(){
		return m_stat.size();
	}
	Sigma< double >&& OrderStatistics::At(size_t i){
		if(i>=count())
			throw Exception<OrderStatistics>("Order statistics range check error");
		return static_cast<Sigma<double>&&>(m_stat[i]);
	}
	void OrderStatistics::AcceptEventStart(){
		m_count=0;
	}
	void OrderStatistics::AcceptPhotonTime(double time){
		if(m_count<m_stat.size())
			m_stat[m_count]<<time;
		m_count++;
	}
	void OrderStatistics::AcceptEventEnd(){}
	
	SignalProducent& SignalProducent::operator>>(shared_ptr<SignalAcceptor> out){
		m_out_slots.push_back(out);
		return *this;
	}
	void SignalProducent::SendEventStart(){
		for(auto out:m_out_slots)
			out->AcceptEventStart();
	}
	void SignalProducent::SendSignalValue(double time){
		for(auto out:m_out_slots)
			out->AcceptSignalValue(time);
	}
	void SignalProducent::SendEventEnd(){
		for(auto out:m_out_slots)
			out->AcceptEventEnd();
	}
	
	WeightedTimeSignal::WeightedTimeSignal(){}
	WeightedTimeSignal::~WeightedTimeSignal(){}
	WeightedTimeSignal& WeightedTimeSignal::AddSummand(size_t order_statistics, double weight){
		m_config.push_back(make_pair(order_statistics,weight));
		return *this;
	}
	void WeightedTimeSignal::AcceptEventStart(){
		m_count=0;
		m_sum=0;
		m_state.clear();
		for(auto element:m_config)m_state.push_back(false);
		SendEventStart();
	}
	void WeightedTimeSignal::AcceptPhotonTime(double time){
		for(int i=0,n=m_config.size();i<n;i++){
			auto p=&(m_config[i]);
			if(p->first==m_count){
				m_sum+=p->second*time;
				m_state[i]=true;
			}
		}
		m_count++;
	}
	void WeightedTimeSignal::AcceptEventEnd(){
		bool todo=true;
		for(bool b:m_state)todo&=b;
		if(todo)
			SendSignalValue(m_sum);
		SendEventEnd();
	}
	AmplitudeSignal::AmplitudeSignal(){}
	AmplitudeSignal::~AmplitudeSignal(){}
	void AmplitudeSignal::AcceptEventStart(){
		m_count=0;
		SendEventStart();
	}
	void AmplitudeSignal::AcceptPhotonTime(double time){
		m_count++;
	}
	void AmplitudeSignal::AcceptEventEnd(){
		SendSignalValue(m_count);
		SendEventEnd();
	}
};