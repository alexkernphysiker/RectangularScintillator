// this file is distributed under 
// GPL v 3.0 license
#include <exception>
#include "math_h/sympson.h"
#include "rectscin.h"
using namespace std;
PhotoSensitiveSurface::PhotoSensitiveSurface(){}
PhotoSensitiveSurface::~PhotoSensitiveSurface(){}
PhotoSensitiveSurface& PhotoSensitiveSurface::operator<<(shared_ptr<IPhotoSensitive> handler){
	m_handlers.push_back(handler);
	return *this;
}
void PhotoSensitiveSurface::Start(){
	for(auto handler:m_handlers)
		handler->Start();
}
void PhotoSensitiveSurface::RegisterPhoton(Photon& photon){
	if(IsInside(static_cast<Vec&&>(photon.coord)))
		for(auto handler:m_handlers)
			handler->RegisterPhoton(photon);
}
void PhotoSensitiveSurface::End(){
	for(auto handler:m_handlers)
		handler->End();
}
RectangularScintillator::RectangularScintillator(
	vector< Pair > dimensions, 
	Func time_distribution, double tmax, unsigned int tbins, 
	Func lambda_distribution, double lmin, double lmax, unsigned int lbins, 
	Func refraction, Func absorption
):RectDimensions(),
m_time_distribution(m_time_distribution,0,tmax,tbins),
m_lambda_distribution(lambda_distribution,lmin,lmax,lbins){
	if(dimensions.size()==0)throw exception();
	for(Pair&D:dimensions)operator<<(static_cast<Pair&&>(D));
	m_refraction=refraction;
	m_absorption=absorption;
	for(unsigned int i=0,n=NumberOfDimensions();i<n;i++){
		SurfPair surfaces;
		for(unsigned int j=0,n=NumberOfDimensions();j<n;j++)
			if(j!=i){
				static_cast<RectDimensions>(surfaces.first)<<Dimension(i);
				static_cast<RectDimensions>(surfaces.second)<<Dimension(i);
			}
		m_edges.push_back(surfaces);
	}
}
RectangularScintillator::~RectangularScintillator(){}
PhotoSensitiveSurface& RectangularScintillator::Surface(unsigned int dimension, IntersectionSearchResults::Side side){
	if(dimension>=NumberOfDimensions())
		throw exception();
	if(side=IntersectionSearchResults::Left)return m_edges[dimension].first;
	if(side=IntersectionSearchResults::Right)return m_edges[dimension].second;
	throw exception();
}
typedef pair<Photon,PhotoSensitiveSurface*> PhotonReg;
bool operator>(PhotonReg&a,PhotonReg&b){return a.first.time>b.first.time;}
bool operator<(PhotonReg&a,PhotonReg&b){return a.first.time<b.first.time;}
void RectangularScintillator::RegisterGamma(Vec&&coord,unsigned int N){
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
		sp.first.Start();
		sp.second.Start();
	}
	for(PhotonReg&reg:photons)
		reg.second->RegisterPhoton(reg.first);
	for(SurfPair&sp:m_edges){
		sp.first.End();
		sp.second.End();
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
		throw exception();//not implemented
	res.time=m_time_distribution();
	res.lambda=m_lambda_distribution();
	return res;
}
double ReflectionProbability(double n, double sin_phi){
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
	double r=Sympson(SubInt,0.0,3.1415926*0.5,0.001);
	if(r<1)return r;
	return 1;
}
RectDimensions::IntersectionSearchResults RectangularScintillator::TraceGeometry(Photon&ph){
	unsigned int cnt=0;
	IntersectionSearchResults res;
	uniform_real_distribution<double> prob_(-1,1);
	double absorption=m_absorption(ph.lambda);
	double n=m_refraction(ph.lambda);
	bool out=false;
	do{
		res=WhereIntersects(static_cast<Vec&&>(ph.coord),static_cast<Vec&&>(ph.dir));
		if(res.Surface==IntersectionSearchResults::None)throw exception();
		double length=Distance(static_cast<Vec&&>(res.Coordinates),static_cast<Vec&&>(ph.coord));
		if(prob_(rand)>exp(-length*absorption)){
			res.Surface=IntersectionSearchResults::None;
			return res;
		}
		double cos_theta=ph.dir[res.SurfaceDimentionIndex];
		ph.time+=length/(speed_of_light*n);
		out=(prob_(rand)>ReflectionProbability(n,sqrt(1.0-cos_theta*cos_theta)));
		if(out){
			ph.dir=static_cast<Vec&&>(ph.dir)*n;
			ph.dir[res.SurfaceDimentionIndex]=0;//it will be deleted
		}else
			ph.dir[res.SurfaceDimentionIndex]*=-1.0;
		cnt++;
		if(cnt>50){
			res.Surface=IntersectionSearchResults::None;
			return res;
		}
	}while(!out);
}
