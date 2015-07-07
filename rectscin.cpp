#include <exception>
#include "rectscin.h"
using namespace std;
PhotoSensitiveSurface::PhotoSensitiveSurface(){}
PhotoSensitiveSurface::~PhotoSensitiveSurface(){}
PhotoSensitiveSurface& PhotoSensitiveSurface::operator<<(shared_ptr<IPhotoSensitive> handler){
	m_handlers.push_back(handler);
	return *this;
}
void PhotoSensitiveSurface::RegisterPhoton(Photon& photon){
	if(IsInside(photon.coord))
		for(auto handler:m_handlers)
			handler->RegisterPhoton(photon);
}
RectangularScintillator::RectangularScintillator(
	vector< Pair > dimensions, 
	Func time_distribution, double tmax, unsigned int tbins, 
	Func lambda_distribution, double lmin, double lmax, unsigned int lbins, 
	Func refraction, Func absorption
):RectDimensions(),
m_time_distribution(m_time_distribution,0,tmax,tbins),
m_lambda_distribution(lambda_distribution,lmin,lmax,lbins){
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
void RectangularScintillator::RegisterGamma(Vec&& coord, unsigned int N){
	for(unsigned int i=0;i<N;i++){
		Photon ph=GeneratePhoton(static_cast<Vec&&>(coord));
		IntersectionSearchResults trace=TraceGeometry(ph);
		if(trace.Surface!=IntersectionSearchResults::None){
			ph.coord.erase(ph.coord.begin()+trace.SurfaceDimentionIndex);
			ph.dir.erase(ph.dir.begin()+trace.SurfaceDimentionIndex);
			Surface(trace.SurfaceDimentionIndex,trace.Surface).RegisterPhoton(ph);
		}
	}
}
Photon RectangularScintillator::GeneratePhoton(Vec&&coord){
	Photon res;
	res.coord=coord;
	res.dir=RandomIsotropic(NumberOfDimensions(),1);
	res.time=m_time_distribution();
	res.lambda=m_lambda_distribution();
	return res;
}
RectDimensions::IntersectionSearchResults RectangularScintillator::TraceGeometry(Photon& ph){
	throw exception();
}
