#include <iostream>
#include "signal_model.h"
using namespace std;
Counter::Counter(){}
Counter::~Counter(){}
double Counter::average(){
	return m_count.getAverage();
}
double Counter::sigma(){
	return m_count.getSigma();
}
unsigned int Counter::count(){
	return current;
}
void Counter::Start(){
	current=0;
}
void Counter::Photon(double){
	current++;
}
void Counter::End(){
	m_count.AddValue(current);
}
void Counter::Reset(){
	m_count=Sigma<double>();
}

Timer::Timer(unsigned int photon_count){
	m_photon_count=photon_count;
	current=0;
}
Timer::~Timer(){}
void Timer::Start(){
	current=0;
	m_time=INFINITY;
}
void Timer::Photon(double t){
	if(current==m_photon_count)
		m_time=t;
	current++;
}
void Timer::End(){
	if(isfinite(m_time))m_count.AddValue(m_time);
}
unsigned int Timer::time(){
	return m_time;
}
double Timer::average(){
	return m_count.getAverage();
}
double Timer::sigma(){
	return m_count.getSigma();
}
void Timer::Reset(){
	m_count=Sigma<double>();
}

TimeDistribution::TimeDistribution(double from, double to, int bins):m_distr(from,to,bins){}
TimeDistribution::~TimeDistribution(){}
Distribution< double >&&TimeDistribution::GetDistribution(){
	return static_cast<Distribution<double>&&>(m_distr);
}
void TimeDistribution::Start(){}
void TimeDistribution::Photon(double t){
	m_distr.AddValue(t);
}
void TimeDistribution::End(){}
