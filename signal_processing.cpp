// this file is distributed under 
// GPL v 3.0 license
#include "math_h/functions.h"
#include "math_h/interpolate.h"
#include "signal_processing.h"
#include "rectscinexception.h"
using namespace std;
void Single2SingleSignal::AcceptEventStart(){SendEventStart();}
void Single2SingleSignal::AcceptEventEnd(){SendEventEnd();}
Signal::Signal(){}
Signal::~Signal(){}
void Signal::AcceptSignalValue(double signal){SendSignalValue(signal);}

SignalPolinomialDistort::SignalPolinomialDistort(Vec&& coefs){
	if(coefs.size()==0)
		throw RectScinException("Distortion: empty coefficients");
	m_coefs=coefs;
}
SignalPolinomialDistort::~SignalPolinomialDistort(){}
void SignalPolinomialDistort::AcceptSignalValue(double signal){
	SendSignalValue(Polynom(signal,m_coefs,m_coefs.size()-1));
}

SignalSmear::SignalSmear(double sigma):smear(0,sigma){}
SignalSmear::~SignalSmear(){}
void SignalSmear::AcceptSignalValue(double signal){
	SendSignalValue(signal+smear(rnd));
}
AmplitudeDiscriminator::AmplitudeDiscriminator(double thr){
	threshold=thr;
}
AmplitudeDiscriminator::~AmplitudeDiscriminator(){}
void AmplitudeDiscriminator::AcceptSignalValue(double signal){
	if(isfinite(signal))
		if(signal>=threshold)
			SendSignalValue(signal);
}


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

SignalSumm::SignalSumm(){}
SignalSumm::~SignalSumm(){}
void SignalSumm::Process(Vec&& signals){
	double val=0;
	for(double signal:signals)
		if(isfinite(signal))
			val+=signal;
		else return;
	SendSignalValue(val);
}
SignalProduct::SignalProduct(){}
SignalProduct::~SignalProduct(){}
void SignalProduct::Process(Vec&& signals){
	double val=1;
	for(double signal:signals)
		if(isfinite(signal))
			val*=signal;
		else return;
	SendSignalValue(val);
}
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

TimeGate::TimeGate(double width){
	if(width<=0)
		throw RectScinException("Wrong time threshold value for TimeGate: it should be positive");
	m_width=width;
}
TimeGate::~TimeGate(){}
void TimeGate::Process(Vec&& signals){
	if(signals.size()>0){
		bool condition=true;
		for(double signal:signals)
			condition&=isfinite(signal);
		if(condition){
			double start=signals[0];
			for(double v:signals){
				if(v<start)return;
				if((v-start)>m_width)return;
			}
			for(size_t i=0;i<signals.size();i++)
				SendSignalValue(i,signals[i]);
		}
	}
}
AllSignalsPresent::AllSignalsPresent(){}
AllSignalsPresent::~AllSignalsPresent(){}
void AllSignalsPresent::Process(Vec&& signals){
	bool condition=true;
	for(double signal:signals)
		condition&=isfinite(signal);
	if(condition)for(size_t i=0;i<signals.size();i++)
		SendSignalValue(i,signals[i]);
}

SignalSortAndSelect2::SignalSortAndSelect2(size_t number){m_number=number;}
SignalSortAndSelect2::~SignalSortAndSelect2(){}
void SignalSortAndSelect2::Process(Vec&& signals){
	if(signals.size()<=m_number)
		throw RectScinException("SignalSortAndSelect: selected order statistics is greater than input slots count");
	Vec out;
	for(double v:signals)if(isfinite(v))
		InsertSorted(v,out,std_size(out),std_insert(out,double));
	if(out.size()>m_number){
		SendSignalValue(0,m_number);
		SendSignalValue(1,out[m_number]);
	}
}
