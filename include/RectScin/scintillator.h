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
	const double speed_of_light=300;//mm/ns
	typedef std::function<double(double)> Func;
	typedef std::mt19937 RANDOM;
	struct Photon{
		Vec coord;
		double time;
		Vec dir;
		double lambda;
	};
	template<class source,class dest>
	const std::shared_ptr<source> operator<<(const std::shared_ptr<source> s,const std::shared_ptr<dest> d){
		s->operator<<(d);
		return s;
	}
	template<class source,class dest>
	const std::shared_ptr<source> operator>>(const std::shared_ptr<source> s,const std::shared_ptr<dest> d){
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
		std::vector<std::shared_ptr<IPhotonAbsorber>> m_handlers;
		std::mutex surface_mutex;
	};
	const double ReflectionProbability(const double refraction,const double cos_);
	class Scintillator:protected RectDimensions{
	public:
		Scintillator(
			const std::vector<Pair>&dimensions,
			const double refraction,
			const MathTemplates::RandomValueGenerator<double>&time_distribution,
			const MathTemplates::RandomValueGenerator<double>&lambda_distribution,
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
		
		const MathTemplates::LinearInterpolation<double>&ReflectionProbabilityFunction()const;
	protected:
		Photon GeneratePhoton(const Vec&coord,RANDOM&R)const;
		IntersectionSearchResults TraceGeometry(Photon &ph,RANDOM&R)const;
	private:
		Options m_config;
		MathTemplates::RandomValueGenerator<double> m_time_distribution;
		MathTemplates::RandomValueGenerator<double> m_lambda_distribution;
		double m_refraction;
		Func m_absorption;//depends on lambda
		MathTemplates::LinearInterpolation<double> reflection_probability;
		typedef std::pair<std::shared_ptr<ScintillatorSurface>,std::shared_ptr<ScintillatorSurface>> SurfPair;
		std::vector<SurfPair> m_edges;
		std::shared_ptr<std::mutex> trace_mutex;
	};
	inline const std::shared_ptr<Scintillator> MakeScintillator(
		const std::vector<Pair>&dimensions,
		double refraction,
		const MathTemplates::RandomValueGenerator<double>&time_distribution,
		const MathTemplates::RandomValueGenerator<double>&lambda_distribution,
		Func absorption
	){
		return std::shared_ptr<Scintillator>(
			new Scintillator(dimensions,refraction,time_distribution,lambda_distribution,absorption)
		);
	}
	inline const std::shared_ptr<Scintillator> MakeScintillator(
		std::vector<Pair>&&dimensions,
		double refraction,
		const MathTemplates::RandomValueGenerator<double>&time_distribution,
		const MathTemplates::RandomValueGenerator<double>&lambda_distribution,
		Func absorption
	){return MakeScintillator(dimensions,refraction,time_distribution,lambda_distribution,absorption);}
	inline const std::shared_ptr<Scintillator> MakeScintillator(
		const std::vector<Pair>&dimensions,
		double refraction,
		const MathTemplates::RandomValueGenerator<double>&time_distribution,
		const MathTemplates::RandomValueGenerator<double>&&lambda_distribution=MathTemplates::RandomValueGenerator<double>(1,2),
		Func absorption=[](double){return 0.0;}
	){return MakeScintillator(dimensions,refraction,time_distribution,lambda_distribution,absorption);}
	inline const std::shared_ptr<Scintillator> MakeScintillator(
		const std::vector<Pair>&&dimensions,
		const double refraction,
		const MathTemplates::RandomValueGenerator<double>&time_distribution,
		const MathTemplates::RandomValueGenerator<double>&&lambda_distribution=MathTemplates::RandomValueGenerator<double>(1,2),
		const Func absorption=[](double){return 0.0;}
	){return MakeScintillator(dimensions,refraction,time_distribution,lambda_distribution,absorption);}
	inline const std::shared_ptr<Scintillator> MakeScintillator(
		const std::vector<Pair>&&dimensions,
		double refraction,
		const MathTemplates::RandomValueGenerator<double>&&time_distribution,
		const MathTemplates::RandomValueGenerator<double>&&lambda_distribution=MathTemplates::RandomValueGenerator<double>(1,2),
		const Func absorption=[](double){return 0.0;}
	){return MakeScintillator(dimensions,refraction,time_distribution,lambda_distribution,absorption);}
	const MathTemplates::RandomValueGenerator<double> TimeDistribution1(double sigma, double decay,const std::vector<double>&&time_chain=MathTemplates::ChainWithStep(0.0,0.01,20.0));
	const MathTemplates::RandomValueGenerator<double> TimeDistribution2(double rize, double sigma, double decay,const std::vector<double>&&time_chain=MathTemplates::ChainWithStep(0.0,0.01,20.0));
};
#endif