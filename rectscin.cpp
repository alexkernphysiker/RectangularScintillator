// this file is distributed under 
// GPL v 3.0 license
#include "math_h/sympson.h"
#include "math_h/functions.h"
#include "rectscinexception.h"
#include "rectscin.h"
using namespace std;
ScintillatorSurface::ScintillatorSurface(){}
ScintillatorSurface::~ScintillatorSurface(){}
ScintillatorSurface& ScintillatorSurface::operator<<(shared_ptr<IPhotoSensitive> handler){
	m_handlers.push_back(handler);
	return *this;
}
void ScintillatorSurface::Start(){
	for(auto handler:m_handlers)
		handler->Start();
}
void ScintillatorSurface::RegisterPhoton(Photon& photon){
	if(IsInside(static_cast<Vec&&>(photon.coord)))
		for(auto handler:m_handlers)
			handler->RegisterPhoton(photon);
}
void ScintillatorSurface::End(){
	for(auto handler:m_handlers)
		handler->End();
}
ScintillatorSurface& ScintillatorSurface::Glue(vector< Pair >&& glue){
	auto glued=make_shared<RectDimensions>();
	for(Pair p:glue)glued->operator<<(static_cast<Pair&&>(p));
	if(glued->NumberOfDimensions()!=NumberOfDimensions())
		throw RectScinException("Glued area dimensions number does not match");
	m_glue.push_back(glued);
	return *this;
}
bool ScintillatorSurface::IsGlued(Vec&&point){
	for(auto glued:m_glue)
		if(glued->IsInside(static_cast<Vec&&>(point)))
			return true;
	return false;
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
	for(Pair D:dimensions)RectDimensions::operator<<(static_cast<Pair&&>(D));
	m_refraction=refraction;
	m_absorption=absorption;
	for(unsigned int i=0,n=NumberOfDimensions();i<n;i++){
		SurfPair surfaces=make_pair(make_shared<ScintillatorSurface>(),make_shared<ScintillatorSurface>());
		for(unsigned int j=0,n=NumberOfDimensions();j<n;j++)
			if(j!=i){
				static_cast<RectDimensions&>(*(surfaces.first))<<Dimension(i);
				static_cast<RectDimensions&>(*(surfaces.second))<<Dimension(i);
			}
		m_edges.push_back(surfaces);
	}
	auto ReflectionProbability=[this](double cos_){
		double cos_phi=cos_;
		if(cos_phi<0.0)cos_phi=-cos_phi;
		if(cos_phi>1.0)cos_phi=1;
		double sin_phi=sqrt(1.0-cos_phi*cos_phi);
		double squrt=sqrt(1-m_refraction*m_refraction*sin_phi*sin_phi);
		double T_ort=(m_refraction*squrt-cos_phi)/(m_refraction*squrt+cos_phi);
		double T_par=(m_refraction*cos_phi-squrt)/(m_refraction*cos_phi+squrt);
		auto SubInt=[T_par,T_ort](double pol){
			return sqrt(pow(sin(pol)*T_ort,2)+pow(cos(pol)*T_par,2));
		};
		double r=Sympson(SubInt,0.0,3.1415926*0.5,0.001);
		if(r<1.0)return r;
		return 1.0;
	};
	for(double x=-0.01;x<=1.01;x+=0.01)
		reflection_probability<<make_pair(x,ReflectionProbability(x));
}
RectangularScintillator::~RectangularScintillator(){}
LinearInterpolation< double >&& RectangularScintillator::ReflectionProbabilityFunction(){
	return static_cast<LinearInterpolation<double>&&>(reflection_probability);
}
ScintillatorSurface& RectangularScintillator::Surface(unsigned int dimension, Side side){
	if(dimension>=NumberOfDimensions())
		throw RectScinException("RectangularScintillator: dimension index out of range");
	if(side==Left)return *(m_edges[dimension].first);
	if(side==Right)return *(m_edges[dimension].second);
	throw RectScinException("RectangularScintillator: surface index out of range");
}
void RectangularScintillator::RegisterGamma(Vec&&coord,unsigned int N){
	for(SurfPair&sp:m_edges){
		sp.first->Start();
		sp.second->Start();
	}
	if(coord.size()!=NumberOfDimensions())
		throw RectScinException("RectangularScintillator: wrong gamma interaction point vector size");
	for(unsigned int i=0;i<N;i++){
		Photon ph=GeneratePhoton(static_cast<Vec&&>(coord));
		IntersectionSearchResults trace=TraceGeometry(ph);
		if(trace.Surface!=None){
			Surface(trace.SurfaceDimentionIndex,trace.Surface).RegisterPhoton(ph);
		}
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
	res.time=m_time_distribution();
	res.lambda=m_lambda_distribution();
	return res;
}
RectDimensions::IntersectionSearchResults RectangularScintillator::TraceGeometry(Photon&ph){
	uniform_real_distribution<double> prob_(0,1);
	while(true){
		IntersectionSearchResults res=
			WhereIntersects(static_cast<Vec&&>(ph.coord),static_cast<Vec&&>(ph.dir));
		if(res.Surface==None)
			return res;
		unsigned int dimension=res.SurfaceDimentionIndex;
		double path_length=res.K;
		//update kinematic parameters
		ph.coord=res.Coordinates;
		ph.time+=path_length*m_refraction/speed_of_light;
		//correct if small errors caused that were a little bit outside
		if(ph.coord[dimension]<Dimension(dimension).first)
			ph.coord[dimension]=Dimension(dimension).first;
		if(ph.coord[dimension]>Dimension(dimension).second)
			ph.coord[dimension]=Dimension(dimension).second;
		//check for other effects
		double absorption_prob=1.0-exp(-path_length*m_absorption(ph.lambda));
		if(prob_(rand)<absorption_prob){
			//Photon is absopbed
			res.Surface=None;
			return res;
		}
		double cos_angle=ph.dir[dimension];
		if(cos_angle<0.0)
			cos_angle=-cos_angle;
		if(cos_angle>1.0)
			throw RectScinException("Trace: cosine error");
		double refl_prob=0;
		{//check if photon reached a glued area of surface
			auto surface=Surface(dimension,res.Surface);
			Vec point=ph.coord;
			point.erase(point.begin()+dimension);
			if(!surface.IsGlued(static_cast<Vec&&>(point)))
				//if not it can be reflected back
				refl_prob=reflection_probability(cos_angle);
		}
		if(prob_(rand)<refl_prob){
			//Photon is reflected back
			ph.dir[dimension]=-ph.dir[dimension];
		}else{
			//Photon leaves the scintillator
			ph.dir=static_cast<Vec&&>(ph.dir)*m_refraction;
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