// this file is distributed under 
// MIT license
#ifndef WEXXxads
#define WEXXxads
#include <functional>
#include <memory>
#include <vector>
#include "../math_h/randomfunc.h"
#include "../math_h/interpolate.h"
#include "geometry.h"
namespace RectangularScintillator{
	using namespace std;
	using namespace MathTemplates;
	const double speed_of_light=300;//mm/ns
	typedef function<double(double)> Func;
	typedef mt19937 RANDOM;
	struct Photon{
		Vec coord;
		double time;
		Vec dir;
		double lambda;
	};
	template<class source,class dest>
	shared_ptr<source> operator<<(shared_ptr<source> s,const shared_ptr<dest> d){
		s->operator<<(d);
		return s;
	}
	template<class source,class dest>
	shared_ptr<source> operator>>(shared_ptr<source> s,const shared_ptr<dest> d){
		s->operator>>(d);
		return s;
	}
	class IPhotonAbsorber{
	public:
		virtual ~IPhotonAbsorber(){}
		virtual void Start()=0;
		virtual void AbsorbPhoton(Photon&photon,RANDOM&R)=0;
		virtual void End()=0;
		virtual const RectDimensions&Dimensions()const=0;
		virtual double GlueEfficiency()const=0;
	};
	class Scintillator;
	class ScintillatorSurface:protected RectDimensions{
		friend class Scintillator;
	public:
		ScintillatorSurface();
		virtual ~ScintillatorSurface();
		ScintillatorSurface&operator>>(const std::shared_ptr<IPhotonAbsorber>sensor);
		const RectDimensions&Dimensions()const;
	protected:
		void Start();
		void RegisterPhoton(Photon&photon,RANDOM&R);//changes photon
		void End();
		const double ReflectionProbabilityCoeff(const Vec&point)const;
	private:
		vector<std::shared_ptr<IPhotonAbsorber>> m_handlers;
		mutex surface_mutex;
	};
	const double ReflectionProbability(const double refraction,const double cos_);
	class Scintillator:protected RectDimensions{
	public:
		Scintillator(
			const vector<Pair>&dimensions,
			const double refraction,
			const RandomValueGenerator<double>&time_distribution,
			const RandomValueGenerator<double>&lambda_distribution,
			const Func absorption
		);
		virtual ~Scintillator();
		ScintillatorSurface&Surface(const size_t dimension,const Side side)const;
		void RegisterGamma(const Vec&&coord,const size_t N,RANDOM&R)const;
		
		struct Options{
			size_t concurrency;
			unsigned long max_reflections;
			Options(const size_t c,const unsigned long refl);
			Options(const Options&&source);
			void operator=(const Options&source);
		};
		static Options Defaults();
		static Options Concurrency(const size_t c);
		static Options Reflections(const unsigned long r);
		void Configure(const Options&&conf);
		const Options&CurrentConfig()const;
		
		const LinearInterpolation<double>&ReflectionProbabilityFunction()const;
	protected:
		Photon GeneratePhoton(const Vec&coord,RANDOM&R)const;
		IntersectionSearchResults TraceGeometry(Photon &ph,RANDOM&R)const;
	private:
		Options m_config;
		RandomValueGenerator<double> m_time_distribution;
		RandomValueGenerator<double> m_lambda_distribution;
		double m_refraction;
		Func m_absorption;//depends on lambda
		LinearInterpolation<double> reflection_probability;
		typedef pair<shared_ptr<ScintillatorSurface>,shared_ptr<ScintillatorSurface>> SurfPair;
		vector<SurfPair> m_edges;
		shared_ptr<mutex> trace_mutex;
	};
	inline shared_ptr<Scintillator> MakeScintillator(
		const vector<Pair>&dimensions,
		double refraction,
		const RandomValueGenerator<double>&time_distribution,
		const RandomValueGenerator<double>&lambda_distribution,
		Func absorption
	){
		return shared_ptr<Scintillator>(
			new Scintillator(dimensions,refraction,time_distribution,lambda_distribution,absorption)
		);
	}
	inline shared_ptr<Scintillator> MakeScintillator(
		vector<Pair>&&dimensions,
		double refraction,
		const RandomValueGenerator<double>&time_distribution,
		const RandomValueGenerator<double>&lambda_distribution,
		Func absorption
	){return MakeScintillator(dimensions,refraction,time_distribution,lambda_distribution,absorption);}
	inline shared_ptr<Scintillator> MakeScintillator(
		const vector<Pair>&dimensions,
		double refraction,
		const RandomValueGenerator<double>&time_distribution,
		RandomValueGenerator<double>&&lambda_distribution=RandomValueGenerator<double>(1,2),
		Func absorption=[](double){return 0.0;}
	){return MakeScintillator(dimensions,refraction,time_distribution,lambda_distribution,absorption);}
	inline shared_ptr<Scintillator> MakeScintillator(
		const vector<Pair>&&dimensions,
		const double refraction,
		const RandomValueGenerator<double>&time_distribution,
		const RandomValueGenerator<double>&&lambda_distribution=RandomValueGenerator<double>(1,2),
		const Func absorption=[](double){return 0.0;}
	){return MakeScintillator(dimensions,refraction,time_distribution,lambda_distribution,absorption);}
	inline shared_ptr<Scintillator> MakeScintillator(
		const vector<Pair>&&dimensions,
		double refraction,
		const RandomValueGenerator<double>&&time_distribution,
		const RandomValueGenerator<double>&&lambda_distribution=RandomValueGenerator<double>(1,2),
		const Func absorption=[](double){return 0.0;}
	){return MakeScintillator(dimensions,refraction,time_distribution,lambda_distribution,absorption);}
	RandomValueGenerator<double> TimeDistribution1(double sigma, double decay,const vector<double>&&time_chain=ChainWithStep(0.0,0.01,20.0));
	RandomValueGenerator<double> TimeDistribution2(double rize, double sigma, double decay,const vector<double>&&time_chain=ChainWithStep(0.0,0.01,20.0));
};
#endif