// this file is distributed under 
// GPL v 3.0 license
#ifndef mxhvHUgn
#define mxhvHUgn
#include "math_h/sigma.h"
#include "math_h/interpolate.h"
#include "math_h/gnuplot/gnuplot.h"
#include "photon2signal.h"
#include "signal_processing.h"
class SignalStatictics:public SignalAcceptor,public Sigma<double>{
public:
	SignalStatictics();
	virtual ~SignalStatictics();
	virtual void AcceptEventStart()override;
	virtual void AcceptSignalValue(double time)override;
	virtual void AcceptEventEnd()override;
};
class SignalDistribution:public SignalAcceptor{
public:
	SignalDistribution(double from, double to, int bincount);
	virtual ~SignalDistribution();
	void PlotAndClear(std::string name);
	virtual void AcceptEventStart()override;
	virtual void AcceptSignalValue(double time)override;
	virtual void AcceptEventEnd()override;
private:
	Plot<double> m_plotter;
	Distribution<double> m_data;
	double f,t;
	int cnt;
};
class Signal2DCorrelation:public AbstractMultiInput{
public:
	Signal2DCorrelation();
	virtual ~Signal2DCorrelation();
	void PlotAndClear(std::string name);
protected:
	virtual void Start()final;
	virtual void Process(Vec&&signals)final;
	virtual void Finish()final;
private:
	std::vector<Pair> m_data;
	PlotPoints<double,decltype(m_data)> m_plotter;
}; 
#endif 