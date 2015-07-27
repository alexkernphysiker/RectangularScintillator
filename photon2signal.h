// this file is distributed under 
// GPL v 3.0 license
#ifndef oRsHdaNh
#define oRsHdaNh
#include "math_h/sigma.h"
#include "sensitive.h"
class OrderStatistics:public PhotonTimeAcceptor{
public:
	OrderStatistics(size_t count);
    virtual ~OrderStatistics();
	size_t count();
	Sigma<double>&& At(size_t i);
	virtual void AcceptEventStart()override;
	virtual void AcceptPhotonTime(double time)override;
	virtual void AcceptEventEnd()override;
private:
	size_t m_count;
	std::vector<Sigma<double>> m_stat;
};
class SignalAcceptor{
public:
	virtual ~SignalAcceptor(){}
	virtual void AcceptEventStart()=0;
	virtual void AcceptSignalValue(double signal)=0;
	virtual void AcceptEventEnd()=0;
};
class SignalProducent{
public:
	virtual ~SignalProducent(){}
	SignalProducent&operator>>(std::shared_ptr<SignalAcceptor>);
protected:
	void SendEventStart();
	void SendSignalValue(double time);
	void SendEventEnd();
private:
	std::vector<std::shared_ptr<SignalAcceptor>> m_out_slots;
};
class WeightedTimeSignal:public PhotonTimeAcceptor,public SignalProducent{
public:
	WeightedTimeSignal();
	virtual ~WeightedTimeSignal();
	WeightedTimeSignal&AddSummand(size_t order_statistics,double weight);
	virtual void AcceptEventStart()override;
	virtual void AcceptPhotonTime(double time)override;
	virtual void AcceptEventEnd()override;
private:
	std::vector<std::pair<size_t,double>> m_config;
	size_t m_count;
	double m_sum;
};
//There's a problem with transfering vector<smth>&& parameter as {val1,val2,...} to make_Shared template function
inline std::shared_ptr<WeightedTimeSignal> TimeSignal(std::vector<std::pair<size_t,double>>&&params){
	auto res=std::make_shared<WeightedTimeSignal>();
	for(auto p:params)
		res->AddSummand(p.first,p.second);
	return res;
}
class AmplitudeSignal:public PhotonTimeAcceptor,public SignalProducent{
public:
	AmplitudeSignal();
	virtual ~AmplitudeSignal();
	virtual void AcceptEventStart()override;
	virtual void AcceptPhotonTime(double time)override;
	virtual void AcceptEventEnd()override;
private:
	size_t m_count;
};
#endif 