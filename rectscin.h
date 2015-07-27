// this file is distributed under 
// GPL v 3.0 license
#ifndef WEXXxads
#define WEXXxads
#include <functional>
#include <memory>
#include "math_h/randomfunc.h"
#include "math_h/interpolate.h"
#include "geometry.h"
const double speed_of_light=300;//mm/ns
typedef std::function<double(double)> Func;
struct Photon{
	Vec coord;
	double time;
	Vec dir;
	double lambda;
};
template<class source,class dest>
std::shared_ptr<source> operator<<(std::shared_ptr<source> s,std::shared_ptr<dest> d){
	s->operator<<(d);
	return s;
}
template<class source,class dest>
std::shared_ptr<source> operator>>(std::shared_ptr<source> s,std::shared_ptr<dest> d){
	s->operator>>(d);
	return s;
}
class IPhotoSensitive{
public:
	virtual ~IPhotoSensitive(){}
	virtual void Start()=0;
	virtual void RegisterPhoton(Photon&photon)=0;
	virtual void End()=0;
	virtual RectDimensions&&Dimensions()=0;
	virtual double GlueEfficiency()=0;
};
class RectangularScintillator;
class ScintillatorSurface:protected RectDimensions{
	friend class RectangularScintillator;
public:
	ScintillatorSurface();
	virtual ~ScintillatorSurface();
	ScintillatorSurface&operator>>(std::shared_ptr<IPhotoSensitive>sensor);
protected:
	void Start();
	void RegisterPhoton(Photon&photon);//changes photon
	void End();
	double ReflectionProbabilityCoeff(Vec&&point);
private:
	std::vector<std::shared_ptr<IPhotoSensitive>> m_handlers;
	std::mutex surface_mutex;
};
double ReflectionProbability(double refraction,double cos_);
class RectangularScintillator:protected RectDimensions{
public:
	RectangularScintillator(
		std::vector<Pair>&&dimensions,
		RandomValueGenerator<double>&&time_distribution,
		RandomValueGenerator<double>&&lambda_distribution,
		double refraction,Func absorption
	);
	virtual ~RectangularScintillator();
	ScintillatorSurface&Surface(size_t dimension,Side side);
	void RegisterGamma(Vec&&coord,size_t N);
	LinearInterpolation<double>&&ReflectionProbabilityFunction();
protected:
	Photon GeneratePhoton(Vec&&coord);
	IntersectionSearchResults TraceGeometry(Photon &ph);//Changes Photon
private:
	RandomValueGenerator<double> m_time_distribution;
	RandomValueGenerator<double> m_lambda_distribution;
	double m_refraction;
	Func m_absorption;//depends on lambda
	LinearInterpolation<double> reflection_probability;
	typedef std::pair<std::shared_ptr<ScintillatorSurface>,std::shared_ptr<ScintillatorSurface>> SurfPair;
	std::vector<SurfPair> m_edges;
	std::default_random_engine rand;
	std::mutex trace_mutex;
};
const double max_emission_time=20;
const double emission_time_binwidth=0.01;
RandomValueGenerator<double> TimeDistribution1(double sigma, double decay,double maxtime=max_emission_time,double dt=emission_time_binwidth);
RandomValueGenerator<double> TimeDistribution2(double rize, double sigma, double decay,double maxtime=max_emission_time,double dt=emission_time_binwidth);
#endif