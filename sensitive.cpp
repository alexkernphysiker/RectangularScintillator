#include "sensitive.h"
using namespace std;
PhotoSensitiveSurface::PhotoSensitiveSurface(vector< Pair >&&dimensions, Func efficiency):RectDimensions(){
	for(Pair&D:dimensions)RectDimensions::operator<<(static_cast<Pair&&>(D));
	m_efficiency=efficiency;
}
shared_ptr<PhotoSensitiveSurface> Photosensor(vector< Pair >&& dimensions, Func efficiency){
	PhotoSensitiveSurface *surf=new PhotoSensitiveSurface(static_cast<vector<Pair>&&>(dimensions),efficiency);
	return shared_ptr<PhotoSensitiveSurface>(surf);
}

PhotoSensitiveSurface::~PhotoSensitiveSurface(){}
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
}
void PhotoSensitiveSurface::RegisterPhoton(Photon& photon){
	if(IsInside(static_cast<Vec&&>(photon.coord))){
		uniform_real_distribution<double> P(0,1);
		if(P(rand)<m_efficiency(photon.lambda))
			PhotonTimeAccepted(photon.time);
	}
}
void PhotoSensitiveSurface::PhotonTimeAccepted(double time){
	for(auto sig:m_signal)
		sig->Photon(time);
}
void PhotoSensitiveSurface::End(){
	for(auto sig:m_signal)
		sig->End();
}