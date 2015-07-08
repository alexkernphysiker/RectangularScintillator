#include <iostream>
#include "signal.h"
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
	printf("START\n");
}
void Counter::Photon(double){
	current++;
}
void Counter::End(){
	m_count.AddValue(current);
	printf("STOP\n");
}



