// this file is distributed under 
// GPL v 3.0 license
#include "math_h/functions.h"
#include "math_h/interpolate.h"
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
void SignalPolinomialDistort::AcceptSignalValue(double signal){
	SendSignalValue(Polynom(signal,m_coefs,m_coefs.size()-1));
}
void SignalPolinomialDistort::AcceptEventEnd(){SendEventEnd();}

SignalSmear::SignalSmear(double sigma):smear(0,sigma){}
SignalSmear::~SignalSmear(){}
void SignalSmear::AcceptEventStart(){SendEventStart();}
void SignalSmear::AcceptSignalValue(double signal){
	SendSignalValue(signal+smear(rnd));
}
void SignalSmear::AcceptEventEnd(){SendEventEnd();}

AbstractMultiInput::AbstractMultiInput(){m_state=0;}
AbstractMultiInput::~AbstractMultiInput(){}
AbstractMultiInput& AbstractMultiInput::operator<<(shared_ptr<SignalProducent> input){
	auto slot=make_shared<Slot>(shared_from_this());
	input>>slot;
	m_input_slots.push_back(slot);
	m_state=0;
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
AbstractMultiInput::Slot::Slot(std::shared_ptr<AbstractMultiInput>father){master=father;}
AbstractMultiInput::Slot::~Slot(){}
void AbstractMultiInput::Slot::AcceptEventStart(){master->OneChannelBegin();m_value=INFINITY;}
void AbstractMultiInput::Slot::AcceptSignalValue(double signal){m_value=signal;}
void AbstractMultiInput::Slot::AcceptEventEnd(){master->OneChannelEnd();}
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
		if(isfinite(signals[i]))
			val+=m_weights[i]*signals[i];
		else return;
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
		if(isfinite(signals[i]))
			val+=pow(signals[i],m_powers[i]);
		else return;
	SendSignalValue(val);
}

AbstractMultiOutput::AbstractMultiOutput(){}
AbstractMultiOutput::~AbstractMultiOutput(){}
AbstractMultiOutput& AbstractMultiOutput::operator>>(shared_ptr< SignalAcceptor > output){
	m_output_slots.push_back(make_shared<Slot>()>>output);
	return *this;
}
void AbstractMultiOutput::SendEventStart(){
	for(auto slot:m_output_slots)
		slot->Start();
}
size_t AbstractMultiOutput::GetOutSlotsCount(){
	return m_output_slots.size();
}
void AbstractMultiOutput::SendSignalValue(size_t i, double signal){
	if(i>=m_output_slots.size())
		throw RectScinException("Mutli output range check error.");
	m_output_slots[i]->Value(signal);
}
void AbstractMultiOutput::SendEventEnd(){
	for(auto slot:m_output_slots)
		slot->End();
}
AbstractMultiOutput::Slot::Slot(){}
AbstractMultiOutput::Slot::~Slot(){}
void AbstractMultiOutput::Slot::Start(){SendEventStart();}
void AbstractMultiOutput::Slot::Value(double signal){SendSignalValue(signal);}
void AbstractMultiOutput::Slot::End(){SendEventEnd();}

Single2MultiSignal::Single2MultiSignal(){}
Single2MultiSignal::~Single2MultiSignal(){}
void Single2MultiSignal::AcceptEventStart(){SendEventStart();}
void Single2MultiSignal::AcceptEventEnd(){SendEventEnd();}

Multi2MultiSignal::Multi2MultiSignal(){}
Multi2MultiSignal::~Multi2MultiSignal(){}
void Multi2MultiSignal::Start(){SendEventStart();}
void Multi2MultiSignal::Finish(){SendEventEnd();}

SignalSortAndSelect::SignalSortAndSelect(size_t number){m_number=number;}
SignalSortAndSelect::~SignalSortAndSelect(){}
void SignalSortAndSelect::Process(Vec&& signals){
	if(signals.size()<=m_number)
		throw RectScinException("SignalSortAndSelect: selected order statistics is greater than input slots count");
	Vec out;
	for(double v:signals)if(isfinite(v))
		InsertSorted(v,out,std_size(out),std_insert(out,double));
	if(out.size()>m_number)
		SendSignalValue(out[m_number]);
}
