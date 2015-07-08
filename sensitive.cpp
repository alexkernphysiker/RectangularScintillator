#include "sensitive.h"
using namespace std;
Photosensor::Photosensor(vector< Pair > dimensions, Func efficiency, double tts):m_tts(0.0,tts){
	for(Pair&D:dimensions)RectDimensions::operator<<(static_cast<Pair&&>(D));
	m_efficiency=efficiency;
}
Photosensor::~Photosensor(){}
Photosensor& Photosensor::operator<<(shared_ptr<ISignal> sig){
	m_signal.push_back(sig);
}
void Photosensor::Start(){
	for(auto sig:m_signal)
		sig->Start();
}
void Photosensor::RegisterPhoton(Photon& photon){
	if(IsInside(static_cast<Vec&&>(photon.coord))){
		uniform_real_distribution<double> P(0,1);
		if(P(rand)<m_efficiency(photon.lambda))
			for(auto sig:m_signal)
				sig->Photon(photon.time+m_tts(rand));
	}
}
void Photosensor::End(){
	for(auto sig:m_signal)
		sig->End();
}