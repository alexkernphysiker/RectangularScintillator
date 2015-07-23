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
class SignalDistribution:public SignalAcceptor,Distribution<double>{
public:
	SignalDistribution(std::string name, double from, double to, int bincount);
	virtual ~SignalDistribution();
	virtual void AcceptEventStart()override;
	virtual void AcceptSignalValue(double time)override;
	virtual void AcceptEventEnd()override;
private:
	Plot<double> m_plotter;
	std::string m_name;
};
class Signal2DCorrelation:public AbstractMultiInput{
public:
	Signal2DCorrelation(std::string name);
	virtual ~Signal2DCorrelation();
protected:
	virtual void Start()final;
	virtual void Process(Vec&&signals)final;
	virtual void Finish()final;
private:
	std::string m_name;
	std::vector<Pair> m_data;
	PlotPoints<double,decltype(m_data)> m_plotter;
}; 
#endif 