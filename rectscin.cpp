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
RectangularScintillator::RectangularScintillator(
	std::vector<Pair>&&dimensions,
	RandomValueGenerator<double>&&time_distribution,
	RandomValueGenerator<double>&&lambda_distribution,
	Func refraction,Func absorption
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
}
RectangularScintillator::~RectangularScintillator(){}
ScintillatorSurface& RectangularScintillator::Surface(unsigned int dimension, IntersectionSearchResults::Side side){
	if(dimension>=NumberOfDimensions())
		throw RectScinException("RectangularScintillator: dimension index out of range");
	if(side==IntersectionSearchResults::Left)return *(m_edges[dimension].first);
	if(side==IntersectionSearchResults::Right)return *(m_edges[dimension].second);
	throw RectScinException("RectangularScintillator: surface index out of range");
}
typedef pair<Photon,ScintillatorSurface*> PhotonReg;
bool operator>(PhotonReg&a,PhotonReg&b){return a.first.time>b.first.time;}
bool operator<(PhotonReg&a,PhotonReg&b){return a.first.time<b.first.time;}
void RectangularScintillator::RegisterGamma(Vec&&coord,unsigned int N){
	if(coord.size()!=NumberOfDimensions())
		throw RectScinException("RectangularScintillator: wrong gamma interaction point vector size");
	vector<PhotonReg> photons;
	for(unsigned int i=0;i<N;i++){
		Photon ph=GeneratePhoton(static_cast<Vec&&>(coord));
		IntersectionSearchResults trace=TraceGeometry(ph);
		if(trace.Surface!=IntersectionSearchResults::None){
			ph.coord.erase(ph.coord.begin()+trace.SurfaceDimentionIndex);
			ph.dir.erase(ph.dir.begin()+trace.SurfaceDimentionIndex);
			PhotonReg reg=make_pair(ph,&Surface(trace.SurfaceDimentionIndex,trace.Surface));
			InsertSorted(reg,photons,std_size(photons),std_insert(photons,PhotonReg));
		}
	}
	for(SurfPair&sp:m_edges){
		sp.first->Start();
		sp.second->Start();
	}
	for(PhotonReg&reg:photons)
		reg.second->RegisterPhoton(reg.first);
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
	unsigned int cnt=0;
	IntersectionSearchResults res;
	uniform_real_distribution<double> prob_(-1,1);
	double absorption=m_absorption(ph.lambda);
	double n=m_refraction(ph.lambda);
	auto ReflectionProbability=[n](double sin_phi){
		double r;
		try{
		auto T_ort=[n](double sin_phi){
			double ssq=sin_phi*sin_phi;
			double cos_phi=::sqrt(1-ssq);
			double squrt=::sqrt(1-n*n*ssq);
			return (n*squrt-cos_phi)/(n*squrt+cos_phi);
		};
		auto T_par=[n](double sin_phi){
			double ssq=sin_phi*sin_phi;
			double cos_phi=::sqrt(1-ssq);
			double squrt=::sqrt(1-n*n*ssq);
			return (n*cos_phi-squrt)/(n*cos_phi+squrt);
		};
		auto SubInt=[sin_phi,T_par,T_ort](double pol){
			return ::sqrt(::pow(sin(pol)*T_ort(sin_phi),2)+::pow(cos(pol)*T_par(sin_phi),2));
		};
		r=Sympson(SubInt,0.0,3.1415926*0.5,0.001);
		}catch(exception){
			throw RectScinException("Reflection probability");
		}
		if(r<1.0)return r;
		return 1.0;
	};
	while(true){
		res=WhereIntersects(static_cast<Vec&&>(ph.coord),static_cast<Vec&&>(ph.dir));
		if(res.Surface==IntersectionSearchResults::None){
			res.Surface=IntersectionSearchResults::None;
			return res;
		}
		double length=Distance(static_cast<Vec&&>(res.Coordinates),static_cast<Vec&&>(ph.coord));
		if(prob_(rand)>exp(-length*absorption)){
			res.Surface=IntersectionSearchResults::None;
			return res;
		}
		ph.time+=length/(speed_of_light*n);
		bool out=(prob_(rand)>ReflectionProbability(sqrt(1.0-pow(ph.dir[res.SurfaceDimentionIndex],2))));
		if(out){
			ph.dir=static_cast<Vec&&>(ph.dir)*n;
			ph.dir[res.SurfaceDimentionIndex]=0;//this dimension will be deleted
			return res;
		}else
			ph.dir[res.SurfaceDimentionIndex]*=-1.0;
		cnt++;
		if(cnt>50){
			res.Surface=IntersectionSearchResults::None;
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