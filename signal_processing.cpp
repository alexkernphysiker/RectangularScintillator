// this file is distributed under 
// GPL v 3.0 license
#include "math_h/functions.h"
#include "signal_processing.h"
#include "rectscinexception.h"
using namespace std;
SignalPolinomialDistort::SignalPolinomialDistort(Vec&& coefs){
	if(coefs.size()==0)
		throw RectScinException("Distortion: empty coefficients");
	m_coefs=coefs;
}
SignalPolinomialDistort::~SignalPolinomialDistort(){}
void SignalPolinomialDistort::AcceptEventStart(){SendEventStart();}
void SignalPolinomialDistort::AcceptSignalValue(double time){
	SendSignalValue(Polynom(time,m_coefs,m_coefs.size()-1));
}
void SignalPolinomialDistort::AcceptEventEnd(){SendEventEnd();}

AbstractMultiInput::AbstractMultiInput(){m_state=0;}
AbstractMultiInput::~AbstractMultiInput(){}
AbstractMultiInput& AbstractMultiInput::operator<<(shared_ptr< SignalProducent > input){
	auto slot=make_shared<Slot>(this);
	input>>slot;
	m_input_slots.push_back(slot);
	return *this;
}
void AbstractMultiInput::OneChannelBegin(){
	if(m_state<0)
		throw RectScinException("MultiInput error: state changed to an invalid value.");
	if(m_state==0)
		Start();
	m_state++;
}
void AbstractMultiInput::OneChannelEnd(){
	m_state--;
	if(m_state==0){
		Vec values;
		for(auto slot:m_input_slots)
			values.push_back(slot->Value());
		Process(static_right(values));
		Finish();
	}
	if(m_state<0)
		throw RectScinException("MultiInput error: state changed to an invalid value.");
}
AbstractMultiInput::Slot::Slot(AbstractMultiInput* father){
	master=father;
}
AbstractMultiInput::Slot::~Slot(){}
void AbstractMultiInput::Slot::AcceptEventStart(){
	master->OneChannelBegin();
}
void AbstractMultiInput::Slot::AcceptSignalValue(double time){
	m_value=time;
}
void AbstractMultiInput::Slot::AcceptEventEnd(){
	master->OneChannelEnd();
}
double AbstractMultiInput::Slot::Value(){return m_value;}

Multi2SingleSignal::Multi2SingleSignal(){}
Multi2SingleSignal::~Multi2SingleSignal(){}
void Multi2SingleSignal::Start(){SendEventStart();}
void Multi2SingleSignal::Finish(){SendEventEnd();}

SumWithWeights::SumWithWeights(Vec&& weights){m_weights=weights;}
SumWithWeights::~SumWithWeights(){}
void SumWithWeights::Process(Vec&& signals){
	size_t n=signals.size();
	if(n>m_weights.size())
		n=m_weights.size();
	double val=0;
	for(size_t i=0;i<n;i++)
		val+=m_weights[i]*signals[i];
	SendSignalValue(val);
}
ProductWithPowers::ProductWithPowers(Vec&& powers){m_powers=powers;}
ProductWithPowers::~ProductWithPowers(){}
void ProductWithPowers::Process(Vec&& signals){
	size_t n=signals.size();
	if(n>m_powers.size())
		n=m_powers.size();
	double val=0;
	for(size_t i=0;i<n;i++)
		val+=pow(signals[i],m_powers[i]);
	SendSignalValue(val);
}

