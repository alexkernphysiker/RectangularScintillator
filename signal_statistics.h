// this file is distributed under 
// GPL v 3.0 license
#ifndef mxhvHUgn
#define mxhvHUgn
#include "math_h/sigma.h"
#include "math_h/interpolate.h"
#include "photon2signal.h"
#include "signal_processing.h"
class SignalStatictics:public SignalAcceptor{
public:
	SignalStatictics();
	virtual ~SignalStatictics();
	virtual void AcceptEventStart()override;
	virtual void AcceptSignalValue(double time)override;
	virtual void AcceptEventEnd()override;
	Sigma<double>&&data();
	void Clear();
private:
	Sigma<double> m_data;
};
class SignalDistribution:public SignalAcceptor{
public:
	SignalDistribution(double from, double to, int bincount);
	virtual ~SignalDistribution();
	virtual void AcceptEventStart()override;
	virtual void AcceptSignalValue(double time)override;
	virtual void AcceptEventEnd()override;
	Distribution<double>&&data();
	void Clear();
private:
	double f,t;int cnt;
	Distribution<double> m_data;
};
class Signal2DCorrelation:public AbstractMultiInput{
public:
	Signal2DCorrelation();
	virtual ~Signal2DCorrelation();
	void Clear();
	std::vector<Pair>&&Points();
protected:
	virtual void Start()final;
	virtual void Process(Vec&&signals)final;
	virtual void Finish()final;
private:
	std::vector<Pair> m_data;
}; 
#endif 