#include "math_h/interpolate.h"
#include "rectscinexception.h"
#include "sensitive.h"
using namespace std;
PhotoSensitiveSurface::PhotoSensitiveSurface(vector< Pair >&&dimensions, Func efficiency):RectDimensions(),P(0,1){
	for(Pair&D:dimensions)RectDimensions::operator<<(static_cast<Pair&&>(D));
	m_efficiency=efficiency;
}
PhotoSensitiveSurface::~PhotoSensitiveSurface(){}
RectDimensions&& PhotoSensitiveSurface::Dimensions(){
	return static_cast<RectDimensions&&>(*this);
}
double PhotoSensitiveSurface::GlueEfficiency(){
	return 1;
}
PhotoSensitiveSurface& PhotoSensitiveSurface::operator<<(shared_ptr<ISignal> sig){
	m_signal.push_back(sig);
	return *this;
}
shared_ptr<PhotoSensitiveSurface> operator<<(shared_ptr<PhotoSensitiveSurface> A, shared_ptr<ISignal> b){
	A->operator<<(b);
	return A;
}
void PhotoSensitiveSurface::Start(){
	for(auto sig:m_signal)
		sig->Start();
	times.clear();
}
void PhotoSensitiveSurface::RegisterPhoton(Photon& photon){
	if(IsInside(static_left(photon.coord))){
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
			sig->Photon(time);
	for(auto sig:m_signal)
		sig->End();
}
PhotoSensitiveSurfaceWithTTS::PhotoSensitiveSurfaceWithTTS(vector<Pair>&& dimensions,Func efficiency,double tts):
	PhotoSensitiveSurface(static_left(dimensions),efficiency),m_tts(0,tts){}
PhotoSensitiveSurfaceWithTTS::~PhotoSensitiveSurfaceWithTTS(){}
void PhotoSensitiveSurfaceWithTTS::PhotonTimeAccepted(double time){
    PhotoSensitiveSurface::PhotonTimeAccepted(time+m_tts(rand));
}

shared_ptr<PhotoSensitiveSurface> Photosensor(vector< Pair >&& dimensions, Func efficiency){
	PhotoSensitiveSurface *surf=new PhotoSensitiveSurface(static_left(dimensions),efficiency);
	return shared_ptr<PhotoSensitiveSurface>(surf);
}
shared_ptr<PhotoSensitiveSurfaceWithTTS> Photosensor(vector< Pair >&& dimensions, Func efficiency, double tts){
	PhotoSensitiveSurfaceWithTTS *surf=new PhotoSensitiveSurfaceWithTTS(static_left(dimensions),efficiency,tts);
	return shared_ptr<PhotoSensitiveSurfaceWithTTS>(surf);
}
