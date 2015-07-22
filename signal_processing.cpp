// this file is distributed under 
// GPL v 3.0 license
#include "math_h/functions.h"
#include "signal_processing.h"
#include "rectscinexception.h"
SignalPolinomialDistort::SignalPolinomialDistort(Vec&& coefs){
	if(m_coefs.size()==0)
		throw RectScinException("Distortion: empty coefficients");
	m_coefs=coefs;
}
SignalPolinomialDistort::~SignalPolinomialDistort(){}
void SignalPolinomialDistort::AcceptEventStart(){SendEventStart();}
void SignalPolinomialDistort::AcceptSignalValue(double time){
	SendSignalValue(Polynom(time,m_coefs,m_coefs.size()-1));
}
void SignalPolinomialDistort::AcceptEventEnd(){SendEventEnd();}


