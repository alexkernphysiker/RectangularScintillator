#include "lightguide.h"
using namespace std;
FlatLightguide::FlatLightguide(vector<Pair>&& dimensions, double glue_eff, double n, double H):
ScintillatorSurface(){
	g_eff=glue_eff;
	refr=n;
	height=H;
	for(Pair D:dimensions)
		RectDimensions::operator<<(static_cast<Pair&&>(D));
}
FlatLightguide::~FlatLightguide(){}
FlatLightguide& FlatLightguide::operator>>(shared_ptr<IPhotonAbsorber> sensor){
	ScintillatorSurface::operator>>(sensor);
	return *this;
}
double FlatLightguide::GlueEfficiency()const{return g_eff;}
RectDimensions&&FlatLightguide::Dimensions(){
	return static_cast<RectDimensions&&>(*this);
}
void FlatLightguide::Start(){
	ScintillatorSurface::Start();
}
void FlatLightguide::AbsorbPhoton(Photon& photon,RANDOM&R){
	photon.dir=photon.dir*(1.0/refr);
	double horiz=Abs(photon.dir);
	double vert=sqrt(1.0-horiz*horiz);
	Vec offset=photon.dir*(horiz*height/vert);
	photon.coord=photon.coord+offset;
	ScintillatorSurface::RegisterPhoton(photon,R);
}
void FlatLightguide::End(){
	ScintillatorSurface::End();
}
