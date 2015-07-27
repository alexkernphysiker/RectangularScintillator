// this file is distributed under 
// GPL v 3.0 license
#include <thread>
#include <mutex>
#include "math_h/sympson.h"
#include "math_h/functions.h"
#include "rectscinexception.h"
#include "rectscin.h"
using namespace std;
ScintillatorSurface::ScintillatorSurface(){}
ScintillatorSurface::~ScintillatorSurface(){}
void ScintillatorSurface::Start(){
	Lock lock(surface_mutex);
	for(auto handler:m_handlers)
		handler->Start();
}
void ScintillatorSurface::RegisterPhoton(Photon& photon){
	if(IsInside(static_right(photon.coord))){
		Lock lock(surface_mutex);
		for(auto handler:m_handlers)
			handler->AbsorbPhoton(photon);
	}
}
void ScintillatorSurface::End(){
	Lock lock(surface_mutex);
	for(auto handler:m_handlers)
		handler->End();
}
ScintillatorSurface& ScintillatorSurface::operator>>(shared_ptr<IPhotonAbsorber> sensor){
	Lock lock(surface_mutex);
	m_handlers.push_back(sensor);
	return *this;
}
double ScintillatorSurface::ReflectionProbabilityCoeff(Vec&& point){
	Lock lock(surface_mutex);
	for(auto sensor:m_handlers)
		if(sensor->Dimensions().IsInside(static_right(point))){
			double eff=sensor->GlueEfficiency();
			if((eff<0)||(eff>1))
				throw RectScinException("Optical glue efficiency must be in the range [0;1]");
			return 1.0-eff;
		}
	return 1.0;
}
double ReflectionProbability(double refraction,double cos_){
	double cos_phi=cos_;
	if(cos_phi<0.0)cos_phi=-cos_phi;
	if(cos_phi>1.0)cos_phi=1;
	double sin_phi=sqrt(1.0-cos_phi*cos_phi);
	double squrt=sqrt(1-refraction*refraction*sin_phi*sin_phi);
	double T_ort=(refraction*squrt-cos_phi)/(refraction*squrt+cos_phi);
	double T_par=(refraction*cos_phi-squrt)/(refraction*cos_phi+squrt);
	auto SubInt=[T_par,T_ort](double pol){
		return sqrt(pow(sin(pol)*T_ort,2)+pow(cos(pol)*T_par,2));
	};
	double r=Sympson(SubInt,0.0,3.1415926*0.5,0.001);
	if(r<1.0)return r;
	return 1.0;
}
RectangularScintillator::RectangularScintillator(
	std::vector<Pair>&&dimensions,
	RandomValueGenerator<double>&&time_distribution,
	RandomValueGenerator<double>&&lambda_distribution,
	double refraction,Func absorption
):RectDimensions(),
m_time_distribution(time_distribution),
m_lambda_distribution(lambda_distribution){
	if(dimensions.size()==0)
		throw RectScinException("Cannot create scintillator with no dimensions");
	for(Pair D:dimensions)RectDimensions::operator<<(static_right(D));
	m_refraction=refraction;
	m_absorption=absorption;
	for(size_t i=0,n=NumberOfDimensions();i<n;i++){
		SurfPair surfaces=make_pair(make_shared<ScintillatorSurface>(),make_shared<ScintillatorSurface>());
		for(size_t j=0,n=NumberOfDimensions();j<n;j++)
			if(j!=i){
				static_cast<RectDimensions&>(*(surfaces.first))<<Dimension(i);
				static_cast<RectDimensions&>(*(surfaces.second))<<Dimension(i);
			}
		m_edges.push_back(surfaces);
	}
	for(double x=-0.01;x<=1.01;x+=0.01)
		reflection_probability<<make_pair(x,ReflectionProbability(m_refraction,x));
}
RectangularScintillator::~RectangularScintillator(){}
LinearInterpolation< double >&& RectangularScintillator::ReflectionProbabilityFunction(){
	Lock lock(trace_mutex);
	return static_right(reflection_probability);
}
ScintillatorSurface& RectangularScintillator::Surface(size_t dimension, Side side){
	if(dimension>=NumberOfDimensions())
		throw RectScinException("RectangularScintillator: dimension index out of range");
	Lock lock(trace_mutex);
	if(side==Left)return *(m_edges[dimension].first);
	if(side==Right)return *(m_edges[dimension].second);
	throw RectScinException("RectangularScintillator: surface index out of range");
}
void RectangularScintillator::RegisterGamma(Vec&&coord,size_t N){
	for(SurfPair&sp:m_edges){
		sp.first->Start();
		sp.second->Start();
	}
	if(coord.size()!=NumberOfDimensions())
		throw RectScinException("RectangularScintillator: wrong gamma interaction point vector size");
	auto process=[this,&coord](size_t n){
		for(size_t i=0;i<n;i++){
			Photon ph=GeneratePhoton(static_right(coord));
			IntersectionSearchResults trace=TraceGeometry(ph);
			if(trace.surface!=None){
				Surface(trace.surfaceDimentionIndex,trace.surface).RegisterPhoton(ph);
			}
		}
	};
	{
		size_t threads=thread::hardware_concurrency();if(threads==0)threads=1;
		size_t part=N/threads,rest=N%threads;
		vector<shared_ptr<thread>> thread_vector;
		for(size_t i=1;i<threads;i++)
			thread_vector.push_back(make_shared<thread>(process,part));
		process(part+rest);
		for(auto thr:thread_vector)thr->join();
	}
	for(SurfPair&sp:m_edges){
		sp.first->End();
		sp.second->End();
	}
}
Photon RectangularScintillator::GeneratePhoton(Vec&&coord){
	Photon res;
	res.coord=coord;
	if(NumberOfDimensions()==1){
		uniform_int_distribution<int> get_bin(0,1);
		if(get_bin(rand)==0)
			res.dir.push_back(-1);
		else
			res.dir.push_back(1);
	}
	if(NumberOfDimensions()==2){
		uniform_real_distribution<double> get_angle(0,2.0*3.1415926);
		double phi=get_angle(rand);
		res.dir.push_back(sin(phi));
		res.dir.push_back(cos(phi));
	}
	if(NumberOfDimensions()==3){
		uniform_real_distribution<double> get_angle(0,2.0*3.1415926);
		uniform_real_distribution<double> get_secondangle(-1,1);
		double phi=get_angle(rand);
		double cos_theta=get_secondangle(rand);
		double sin_theta=sqrt(1-cos_theta*cos_theta);
		res.dir.push_back(cos_theta);
		res.dir.push_back(sin_theta*sin(phi));
		res.dir.push_back(sin_theta*cos(phi));
	}
	if(NumberOfDimensions()>=4)
		throw RectScinException("RectangularScintillator: isotropic 4D random directions not implemented");
	{
		Lock lock(trace_mutex);
		res.time=m_time_distribution();
		res.lambda=m_lambda_distribution();
	}
	return res;
}
RectDimensions::IntersectionSearchResults RectangularScintillator::TraceGeometry(Photon&ph){
	uniform_real_distribution<double> prob_(0,1);
	double absorption;{
		Lock lock(trace_mutex);
		absorption=m_absorption(ph.lambda);
	}
	double refl_p[NumberOfDimensions()];
	for(size_t dimension=0,n=NumberOfDimensions();dimension<n;dimension++){
		double cos_angle=ph.dir[dimension];
		if(cos_angle<0.0)
			cos_angle=-cos_angle;
		if(cos_angle>1.0)
			throw RectScinException("Trace: cosine error");
		Lock lock(trace_mutex);
		refl_p[dimension]=reflection_probability(cos_angle);
	}
	while(true){
		IntersectionSearchResults res=
			WhereIntersects(static_right(ph.coord),static_right(ph.dir));
		if(res.surface==None)
			return res;
		size_t dimension=res.surfaceDimentionIndex;
		double path_length=res.k;
		//update kinematic parameters
		ph.coord=res.coordinates;
		ph.time+=path_length*m_refraction/speed_of_light;
		//correct if small errors caused that were a little bit outside
		if(ph.coord[dimension]<Dimension(dimension).first)
			ph.coord[dimension]=Dimension(dimension).first;
		if(ph.coord[dimension]>Dimension(dimension).second)
			ph.coord[dimension]=Dimension(dimension).second;
		//check for other effects
		double absorption_prob=1.0-exp(-path_length*absorption);
		if(prob_(rand)<absorption_prob){
			//Photon is absopbed
			res.surface=None;
			return res;
		}
		double refl_prob=refl_p[dimension];
		{//check if photon reached a glued area of surface
			Vec point=ph.coord;
			point.erase(point.begin()+dimension);
			refl_prob*=Surface(dimension,res.surface).ReflectionProbabilityCoeff(static_right(point));
		}
		if(prob_(rand)<refl_prob){
			//Photon is reflected back
			ph.dir[dimension]=-ph.dir[dimension];
		}else{
			//Photon leaves the scintillator
			ph.dir=static_right(ph.dir)*m_refraction;
			ph.coord.erase(ph.coord.begin()+dimension);
			ph.dir.erase(ph.dir.begin()+dimension);
			return res;
		}
	}
}
RandomValueGenerator<double> TimeDistribution1(double sigma, double decay,double maxtime,double dt){
	auto func=[sigma,decay,maxtime,dt](double t){
		auto A=[sigma](double th){if(th<0)return 0.0;return Gaussian(th,2.5*sigma,sigma);};
		auto B=[decay](double th){if(th<0)return 0.0;return exp(-th/decay);};
		return Sympson([t,sigma,decay,A,B](double ksi){return A(ksi)*B(t-ksi);},-maxtime,maxtime,dt);
	};
	return RandomValueGenerator<double>(func,0,maxtime,int(maxtime/dt));
}
RandomValueGenerator<double> TimeDistribution2(double rize,double sigma, double decay,double maxtime,double dt){
	auto func=[rize,sigma,decay,maxtime,dt](double t){
		auto A=[sigma](double th){if(th<0)return 0.0;return Gaussian(th,2.5*sigma,sigma);};
		auto B=[rize,decay](double th){if(th<0)return 0.0;return exp(-th/decay)-exp(-th/rize);};
		return Sympson([t,sigma,decay,A,B](double ksi){return A(ksi)*B(t-ksi);},-maxtime,maxtime,dt);
	};
	return RandomValueGenerator<double>(func,0,maxtime,int(maxtime/dt));
}