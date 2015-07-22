#include "math_h/interpolate.h"
#include "rectscinexception.h"
#include "sensitive.h"
using namespace std;
PhotoSensitiveSurface::PhotoSensitiveSurface(vector< Pair >&&dimensions,double glue,Func efficiency):RectDimensions(),P(0,1){
	for(Pair&D:dimensions)RectDimensions::operator<<(static_cast<Pair&&>(D));
	m_efficiency=efficiency;
	m_glue=glue;
}
PhotoSensitiveSurface::~PhotoSensitiveSurface(){}
RectDimensions&& PhotoSensitiveSurface::Dimensions(){
	return static_cast<RectDimensions&&>(*this);
}
double PhotoSensitiveSurface::GlueEfficiency(){
	return m_glue;
}
PhotoSensitiveSurface& PhotoSensitiveSurface::operator>>(shared_ptr<PhotonTimeAcceptor> sig){
	m_signal.push_back(sig);
	return *this;
}
shared_ptr<PhotoSensitiveSurface> operator>>(shared_ptr<PhotoSensitiveSurface> A, shared_ptr<PhotonTimeAcceptor> b){
	A->operator>>(b);
	return A;
}
void PhotoSensitiveSurface::Start(){
	for(auto sig:m_signal)
		sig->AcceptEventStart();
	times.clear();
}
void PhotoSensitiveSurface::RegisterPhoton(Photon& photon){
	if(IsInside(static_right(photon.coord))){
		if(P(rand)<m_efficiency(photon.lambda))
			PhotonTimeAccepted(photon.time);
	}
}
void PhotoSensitiveSurface::PhotonTimeAccepted(double time){
	InsertSorted(time,times,field_size(times),field_insert(times,double));
}
void PhotoSensitiveSurface::End(){
	for(double time:times)
		for(auto sig:m_signal)
			sig->AcceptPhotonTime(time);
	for(auto sig:m_signal)
		sig->AcceptEventEnd();
}
PhotoSensitiveSurfaceWithTTS::PhotoSensitiveSurfaceWithTTS(vector<Pair>&& dimensions,double glue,Func efficiency,double tts):
	PhotoSensitiveSurface(static_right(dimensions),glue,efficiency),m_tts(0,tts){}
PhotoSensitiveSurfaceWithTTS::~PhotoSensitiveSurfaceWithTTS(){}
void PhotoSensitiveSurfaceWithTTS::PhotonTimeAccepted(double time){
    PhotoSensitiveSurface::PhotonTimeAccepted(time+m_tts(rand));
}
shared_ptr<PhotoSensitiveSurface> Photosensor(vector< Pair >&& dimensions,double glue,Func efficiency){
	PhotoSensitiveSurface *surf=new PhotoSensitiveSurface(static_right(dimensions),glue,efficiency);
	return shared_ptr<PhotoSensitiveSurface>(surf);
}
shared_ptr<PhotoSensitiveSurfaceWithTTS> Photosensor(vector< Pair >&& dimensions,double glue,Func efficiency, double tts){
	PhotoSensitiveSurfaceWithTTS *surf=new PhotoSensitiveSurfaceWithTTS(static_right(dimensions),glue,efficiency,tts);
	return shared_ptr<PhotoSensitiveSurfaceWithTTS>(surf);
}
