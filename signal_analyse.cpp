#include "math_h/interpolate.h"
#include "signal_analyse.h"
#include "rectscinexception.h"
using namespace std;
void ActiveSlotsCount::SetThresholdCount(unsigned int v){m_count=v;}
unsigned int ActiveSlotsCount::ThresholdCount(){return m_count;}
unsigned int ActiveSlotsCount::CurrentCount(){
	unsigned int actual=0;
	for(int i=0,n=AbstractTimeScheme::Count();i<n;i++)
		if(AbstractTimeScheme::LastActive(i))
			actual++;
	return actual;
}
bool ActiveSlotsCount::Condition(){return CurrentCount()>=ThresholdCount();}
bool ActiveSlotsCountExact::Condition(){return CurrentCount()==ThresholdCount();}
void SlotsByNumber::ConfigureSlotsByNumber(vector<unsigned int>&&slot_numbers){
	m_slot_numbers=slot_numbers;
}
bool SlotsByNumber::Condition(){
	for(auto i:m_slot_numbers)
		if(!AbstractTimeScheme::LastActive(i))
			return false;
	return true;
}

void SortedChannels::SetChannelOrderStatistics(unsigned int count){
	m_count=count;
}
Vec&& SortedChannels::LastSortedTime(){
	return static_right(m_times);
}
void SortedChannels::Process(){
	m_times.clear();
	EventStarted();
	for(unsigned int i=0,n=Count();i<n;i++)
		if(LastActive(i))
			InsertSorted(LastTime(i).first,m_times,field_size(m_times),field_insert(m_times,double));
	if(m_times.size()>=m_count)
		Press(m_times[m_count]);
	EventFinished();
}

void WeightedTimesSum::WeightedConfigure(vector<IVal>&&coefs){
	m_coefs=coefs;
	m_sigma=Sigma<double>();
}
Sigma<double>&& WeightedTimesSum::Weighted(){
	return static_right(m_sigma);
}
void WeightedTimesSum::Process(){
	double W=0;
	for(IVal&coef:m_coefs){
		if(!LastActive(coef.first))
			throw RectScinException("Weighted summ error: one of the channels is inactive");
		W+=coef.second*LastTime(coef.first).first;
	}
	m_sigma.AddValue(W);
}
