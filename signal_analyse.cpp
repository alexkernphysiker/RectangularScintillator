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

void SortedChannels::ConfigureSortedChannels(unsigned int count){
	m_sigma.clear();
	for(unsigned int i=0;i<count;i++)
		m_sigma.push_back(Sigma<double>());
}
unsigned int SortedChannels::SortedCount(){
	return m_sigma.size();
}
Sigma<double>&& SortedChannels::SortedSigma(unsigned int index){
	if(index>=SortedCount())throw RectScinException("SortedChannels: range check error");
	return static_cast<Sigma<double>&&>(m_sigma[index]);
}
Vec&& SortedChannels::LastSortedTime(){
	return static_right(m_times);
}
void SortedChannels::Process(){
	m_times.clear();
	for(unsigned int i=0,n=Count();i<n;i++)
		if(LastActive(i))
			InsertSorted(LastTime(i).first,m_times,field_size(m_times),field_insert(m_times,double));
	for(unsigned int i=0,n=m_times.size(),N=SortedCount();(i<n)&&(i<N);i++)
		m_sigma[i].AddValue(m_times[i]);
}

void WeightedTimesSum::WeightedConfigure(vector< IVal >&& coefs){
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
