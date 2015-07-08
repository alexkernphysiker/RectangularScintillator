#include <iostream>
#include "signal_model.h"
Counter::Counter(){}
Counter::~Counter(){}
double Counter::average(){
	return m_count.getAverage();
}
double Counter::sigma(){
	return m_count.getSigma();
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
Timer::Timer(unsigned int photon_threshold){
	m_photon_threshold=photon_threshold;
}
Timer::~Timer(){}
double Timer::average(){
	return m_time.getAverage();
}
double Timer::sigma(){
	return m_time.getSigma();
}
void Timer::Start(){
	current=0;
}
void Timer::Photon(double time){
	if(current==m_photon_threshold)
		m_time.AddValue(time);
	current++;
}
void Timer::End(){}
