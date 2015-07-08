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
