#ifndef CilPeTQJ
#define CilPeTQJ
#include "math_h/sigma.h"
#include "sensitive.h"
class Counter:public ISignal{
public:
	Counter();
    virtual ~Counter();
	virtual void Start()override;
	virtual void Photon(double time)override;
	virtual void End()override;
	double average();
	double sigma();
private:
	unsigned int current;
	Sigma<double> m_count;
};
class Timer:public ISignal{
public:
	Timer(unsigned int photon_threshold);
    virtual ~Timer();
	virtual void Start()override;
	virtual void Photon(double time)override;
	virtual void End()override;
	double average();
	double sigma();
private:
	unsigned int current;
	unsigned int m_photon_threshold;
	Sigma<double> m_time;
};
#endif