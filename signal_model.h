#ifndef CilPeTQJ
#define CilPeTQJ
#include "math_h/sigma.h"
#include "math_h/interpolate.h"
#include "sensitive.h"
class Counter:public ISignal{
public:
	Counter();
    virtual ~Counter();
	virtual void Start()override;
	virtual void Photon(double time)override;
	virtual void End()override;
	unsigned int count();
	double average();
	double sigma();
	void Reset();
private:
	unsigned int current;
	Sigma<double> m_count;
};
class Timer:public ISignal{
public:
	Timer(unsigned int photon_count);
	virtual ~Timer();
	virtual void Start()override;
	virtual void Photon(double t)override;
	virtual void End()override;
	unsigned int time();
	double average();
	double sigma();
	void Reset();
private:
	unsigned int current,m_photon_count;
	double m_time;
	Sigma<double> m_count;
};
class TimeDistribution:public ISignal{
public:
	TimeDistribution(double from,double to, int bins);
	virtual ~TimeDistribution();
	virtual void Start()override;
	virtual void Photon(double t)override;
	virtual void End()override;
	Distribution<double>&&GetDistribution();
private:
	Distribution<double> m_distr;
};
#endif