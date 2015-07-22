#include "lightguide.h"
using namespace std;
FlatLightguide::FlatLightguide(vector<Pair>&& dimensions, double glue_eff, double n, double H):
ScintillatorSurface(){
	g_eff=glue_eff;
	refr=n;
	height=H;
	for(Pair D:dimensions)
		RectDimensions::operator<<(static_right(D));
}
FlatLightguide::~FlatLightguide(){}
FlatLightguide& FlatLightguide::operator>>(shared_ptr< IPhotoSensitive > sensor){
	ScintillatorSurface::operator>>(sensor);
	return *this;
}
double FlatLightguide::GlueEfficiency(){return g_eff;}
RectDimensions&& FlatLightguide::Dimensions(){
	return static_cast<RectDimensions&&>(*this);
}
void FlatLightguide::Start(){
	ScintillatorSurface::Start();
}
void FlatLightguide::RegisterPhoton(Photon& photon){
	photon.dir=static_right(photon.dir)*(1.0/refr);
	double horiz=Abs(static_right(photon.dir));
	double vert=sqrt(1.0-horiz*horiz);
	Vec offset=static_right(photon.dir)*(horiz*height/vert);
	photon.coord=static_right(photon.coord)+static_right(offset);
	ScintillatorSurface::RegisterPhoton(photon);
}
void FlatLightguide::End(){
	ScintillatorSurface::End();
}
shared_ptr< FlatLightguide > operator>>(shared_ptr<FlatLightguide> A, shared_ptr<IPhotoSensitive> sensor){
	A->operator>>(sensor);
	return A;
}
shared_ptr< FlatLightguide > LightGuide(vector< Pair >&& dimensions, double glue_eff, double n, double H){
	FlatLightguide *res=new FlatLightguide(static_right(dimensions),glue_eff,n,H);
	return shared_ptr<FlatLightguide>(res);
}