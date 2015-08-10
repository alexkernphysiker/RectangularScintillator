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
void PhotoSensitiveSurface::Start(){
	for(auto sig:m_signal)
		sig->AcceptEventStart();
	times.clear();
}
void PhotoSensitiveSurface::AbsorbPhoton(Photon& photon,RANDOM&R){
	if(IsInside(static_right(photon.coord)))
		if(P(R)<m_efficiency(photon.lambda))
			PhotonTimeAccepted(photon.time,R);
}
void PhotoSensitiveSurface::PhotonTimeAccepted(double time,RANDOM&R){
	InsertSorted(time,times,field_size(times),field_insert(times,double));
}
void PhotoSensitiveSurface::End(){
	for(double time:times)
		for(auto sig:m_signal)
			sig->AcceptPhotonTime(time);
	for(auto sig:m_signal)
		sig->AcceptEventEnd();
}

PhotoSensitiveSurfaceWithTTS::PhotoSensitiveSurfaceWithTTS(vector<Pair>&& dimensions, double glue, Func efficiency, double tts):
	PhotoSensitiveSurface(static_right(dimensions),glue,efficiency),m_tts(5*tts,tts){}
PhotoSensitiveSurfaceWithTTS::~PhotoSensitiveSurfaceWithTTS(){}
void PhotoSensitiveSurfaceWithTTS::PhotonTimeAccepted(double time, RANDOM& R){
	double offs=m_tts(R);if(offs<0)offs=0;
    PhotoSensitiveSurface::PhotonTimeAccepted(time+offs, R);
}


