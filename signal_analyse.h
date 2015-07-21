#ifndef ORNaBMgU
#define ORNaBMgU
#include "math_h/sigma.h"
#include "signal_model.h"
template<class ConditionProvider,class ProcessProvider>
class Scheme:public virtual ConditionProvider,public virtual ProcessProvider{
public:
	Scheme():AbstractTimeScheme(),ConditionProvider(),ProcessProvider(){}
	virtual ~Scheme(){}
protected:
	virtual bool Condition()override{return ConditionProvider::Condition();}
	virtual void Process()override{ProcessProvider::Process();}
};
template<class Proc1,class Proc2>
class Connect:public virtual Proc1,public virtual Proc2{
public:
	Connect():AbstractTimeScheme(),Proc1(),Proc2(){}
	virtual ~Connect(){}
protected:
	virtual void Process()override{
		Proc1::Process();
		Proc2::Process();
	}
};
class ActiveSlotsCount:public virtual AbstractTimeScheme{
public:
    ActiveSlotsCount(){}
    virtual ~ActiveSlotsCount(){}
	unsigned int ThresholdCount();
	void SetThresholdCount(unsigned int v);
protected:
	unsigned int CurrentCount();
	virtual bool Condition()override;
private:
	unsigned int m_count;
};
class ActiveSlotsCountExact:public virtual ActiveSlotsCount{
	ActiveSlotsCountExact(){}
	virtual ~ActiveSlotsCountExact(){}
protected:
	virtual bool Condition()override;
};
class SlotsByNumber:public virtual AbstractTimeScheme{
public:
    SlotsByNumber(){}
    virtual ~SlotsByNumber(){}
    void ConfigureSlotsByNumber(std::vector<unsigned int>&&slot_numbers);
protected:
	virtual bool Condition()override;
private:
	std::vector<unsigned int> m_slot_numbers;
};
class SortedChannels:public virtual AbstractTimeScheme{
public:
    SortedChannels(){}
    virtual ~SortedChannels(){}
    void ConfigureSortedChannels(unsigned int count);
	unsigned int SortedCount();
	Vec&&LastSortedTime();
	Sigma<double>&&SortedSigma(unsigned int index);
protected:
	virtual void Process()override;
private:
	Vec m_times;
	std::vector<Sigma<double>> m_sigma;
};
typedef std::pair<unsigned int,double> IVal;
class WeightedTimesSum:public virtual AbstractTimeScheme{
public:
	WeightedTimesSum(){}
	virtual ~WeightedTimesSum(){}
	void WeightedConfigure(std::vector<IVal>&&coefs);
	Sigma<double>&&Weighted();
protected:
	virtual void Process()override;
private:
	std::vector<IVal> m_coefs;
	Sigma<double> m_sigma;
};
#endif