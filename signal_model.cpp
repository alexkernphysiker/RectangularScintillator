#include <iostream>
#include "signal_model.h"
#include "rectscinexception.h"
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
unsigned int Counter::events_count(){
	m_count.count();
}
void Counter::Reset(){
	m_count=Sigma<double>();
}

TimeDistribution::TimeDistribution(double from, double to, int bins):m_distr(from,to,bins){}
TimeDistribution::~TimeDistribution(){}
Distribution< double >&&TimeDistribution::GetDistribution(){
	return static_right(m_distr);
}
void TimeDistribution::Start(){}
void TimeDistribution::Photon(double t){
	m_distr.AddValue(t);
}
void TimeDistribution::End(){}

TimeSignalChannel::TimeSignalChannel(Achtung start,Achtung finish){
	m_start=start;m_finish=finish;
	m_state=false;in_event=false;
	m_time=make_pair(INFINITY,INFINITY);
}
TimeSignalChannel::~TimeSignalChannel(){}
bool TimeSignalChannel::InEvent(){return in_event;}
bool TimeSignalChannel::Active(){return m_state;}
Pair&& TimeSignalChannel::Time(){return static_right(m_time);}
void TimeSignalChannel::EventStarted(){
	m_state=false;in_event=true;
	m_time=make_pair(INFINITY,INFINITY);
	m_start();
}
void TimeSignalChannel::Press(double time){
	if(!in_event)throw RectScinException("TimeSignalChannel: signal without event");
	m_time.first=time;
	m_state=true;
}
void TimeSignalChannel::Release(double time){
	if(!in_event)throw RectScinException("TimeSignalChannel: signal without event");
	m_time.second=time;
}
void TimeSignalChannel::EventFinished(){
	in_event=false;
	m_finish();
}

AbstractSignalProducer::~AbstractSignalProducer(){}
AbstractSignalProducer& AbstractSignalProducer::operator<<(shared_ptr< ISignalChannel > channel){
	m_channels.push_back(channel);
	return *this;
}
void AbstractSignalProducer::EventStarted(){
	for(auto channel:m_channels)channel->EventStarted();
}
void AbstractSignalProducer::Press(double time){
	for(auto channel:m_channels)channel->Press(time);
}
void AbstractSignalProducer::Release(double time){
	for(auto channel:m_channels)channel->Release(time);
}
void AbstractSignalProducer::EventFinished(){
	for(auto channel:m_channels)channel->EventFinished();
}
TimeSignal::TimeSignal(unsigned int photon_count){
	m_photon_count=photon_count;
	current=0;
}
TimeSignal::~TimeSignal(){}
void TimeSignal::Start(){
	current=0;
	EventStarted();
}
void TimeSignal::Photon(double t){
	if(current==m_photon_count){
		Press(t);
	}
	current++;
}
void TimeSignal::End(){
	EventFinished();
}
AbstractTimeScheme::AbstractTimeScheme():m_counter(0){}
AbstractTimeScheme::~AbstractTimeScheme(){}
AbstractTimeScheme& AbstractTimeScheme::operator<<(shared_ptr<AbstractSignalProducer> source){
	auto slot=make_shared<TimeSignalChannel>([this](){Start_handler();},[this](){End_handler();});
	source->operator<<(slot);
	m_slots.push_back(slot);
	return *this;
}
shared_ptr<AbstractTimeScheme> operator<<(shared_ptr<AbstractTimeScheme>scheme,shared_ptr<AbstractSignalProducer>slot){
	scheme->operator<<(slot);
	return scheme;
}
unsigned int AbstractTimeScheme::Count(){return m_slots.size();}
bool AbstractTimeScheme::LastActive(unsigned int index){
	if(index>=Count())throw RectScinException("TimeScheme error: slot index out of range");
	return m_slots[index]->Active();
}
Pair&&AbstractTimeScheme::LastTime(unsigned int index){
	if(index>=Count())throw RectScinException("TimeScheme error: slot index out of range");
	return m_slots[index]->Time();
}
void AbstractTimeScheme::Start_handler(){
	m_counter++;
}
void AbstractTimeScheme::End_handler(){
	m_counter--;
	if(m_counter<0)throw RectScinException("TimeScheme error: some slot sent event end signal without start signal");
	if(m_counter==0){
		if(Condition())Process();
	}
}

