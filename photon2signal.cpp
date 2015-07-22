// this file is distributed under 
// GPL v 3.0 license
#include "photon2signal.h"
#include "rectscinexception.h"
using namespace std;
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
		throw RectScinException("Order statistics range check error");
	return static_cast<Sigma<double>&&>(m_stat[i]);
}
void OrderStatistics::AcceptEventStart(){
	m_count=0;
}
void OrderStatistics::AcceptPhotonTime(double time){
	if(m_count<m_stat.size())
		m_stat[m_count].AddValue(time);
	m_count++;
}
void OrderStatistics::AcceptEventEnd(){}

TimeSignalProducent& TimeSignalProducent::operator>>(shared_ptr<TimeSignalAcceptor> out){
	m_out_slots.push_back(out);
	return *this;
}
shared_ptr<TimeSignalProducent> operator>>(shared_ptr< TimeSignalProducent > source, shared_ptr< TimeSignalAcceptor > out){
	source->operator>>(out);
	return source;
}
void TimeSignalProducent::SendEventStart(){
	for(auto out:m_out_slots)
		out->AcceptEventStart();
}
void TimeSignalProducent::SendSignalTime(double time){
	for(auto out:m_out_slots)
		out->AcceptSignalTime(time);
}
void TimeSignalProducent::SendEventEnd(){
	for(auto out:m_out_slots)
		out->AcceptEventEnd();
}

WeightedTimeSignal::WeightedTimeSignal(){}
WeightedTimeSignal::~WeightedTimeSignal(){}
WeightedTimeSignal& WeightedTimeSignal::AddSummand(size_t order_statistics, double weight){
	m_config.push_back(make_pair(order_statistics,weight));
}
void WeightedTimeSignal::AcceptEventStart(){
	m_count=0;
	m_sum=0;
	SendEventStart();
}
void WeightedTimeSignal::AcceptPhotonTime(double time){
	for(auto p:m_config)
		if(p.first==m_count)
			m_sum+=p.second*time;
	m_count++;
}
void WeightedTimeSignal::AcceptEventEnd(){
	SendSignalTime(m_sum);
	SendEventEnd();
}


