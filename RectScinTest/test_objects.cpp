// this file is distributed under 
// GPL v 3.0 license
#include "test_objects.h"
Signal::Signal(){}
Signal::~Signal(){}
void Signal::AcceptEventStart(){
	SendEventStart();
}
void Signal::AcceptSignalValue(double signal){
	SendSignalValue(signal);
}
void Signal::AcceptEventEnd(){
	SendEventEnd();
}

SignalSender::SignalSender(){}
SignalSender::~SignalSender(){}
SignalSender& SignalSender::operator>>(shared_ptr<SignalAcceptor> slot){
	m_slots.push_back(slot);
	return *this;
}
SignalSender& SignalSender::Connect2MultiInput(shared_ptr<AbstractMultiInput> input, size_t count){
	for(size_t i=0;i<count;i++){
		auto signal=make_shared<Signal>();
		operator>>(signal);
		input<<signal;
	}
	return *this;
}
SignalSender& SignalSender::send(Vec&& signals){
	for(shared_ptr<SignalAcceptor> slot:m_slots)
		slot->AcceptEventStart();
	size_t i=0;
	for(shared_ptr<SignalAcceptor> slot:m_slots){
		slot->AcceptSignalValue(signals[i]);
		i++;
	}
	for(shared_ptr<SignalAcceptor> slot:m_slots)
		slot->AcceptEventEnd();
	return *this;
}

Out::Out(){event=false;}Out::~Out(){}
double Out::value(){return signal;}
void Out::AcceptEventStart(){
	if(event)throw RectScinException("Signal producent unusual behaviour");
	event=true;
}
void Out::AcceptSignalValue(double s){
	if(!event)throw RectScinException("Signal producent unusual behaviour");
	signal=s;
}
void Out::AcceptEventEnd(){
	if(!event)throw RectScinException("Signal producent unusual behaviour");
	event=false;
}


