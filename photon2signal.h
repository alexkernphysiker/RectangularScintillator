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
class TimeSignalAcceptor{
public:
	virtual ~TimeSignalAcceptor(){}
	virtual void AcceptEventStart()=0;
	virtual void AcceptSignalTime(double time)=0;
	virtual void AcceptEventEnd()=0;
};
class TimeSignalProducent{
public:
	virtual ~TimeSignalProducent(){}
	TimeSignalProducent&operator>>(std::shared_ptr<TimeSignalAcceptor>);
protected:
	void SendEventStart();
	void SendSignalTime(double time);
	void SendEventEnd();
private:
	std::vector<std::shared_ptr<TimeSignalAcceptor>> m_out_slots;
};
std::shared_ptr<TimeSignalProducent> operator>>(std::shared_ptr<TimeSignalProducent>,std::shared_ptr<TimeSignalAcceptor>);
class MultiTimeSignalAcceptor;

#endif 