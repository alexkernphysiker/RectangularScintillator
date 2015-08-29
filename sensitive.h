#ifndef fXIVCFLg
#define fXIVCFLg
#include "rectscin.h"
class PhotonTimeAcceptor{
public:
	virtual ~PhotonTimeAcceptor(){}
	virtual void AcceptEventStart()=0;
	//Photons come sorted by time
	virtual void AcceptPhotonTime(double time)=0;
	virtual void AcceptEventEnd()=0;
};
class PhotoSensitiveSurface:public IPhotonAbsorber,protected RectDimensions{
public:
	PhotoSensitiveSurface(std::vector<Pair>&&dimensions,double glue,Func efficiency);
	virtual ~PhotoSensitiveSurface();
	virtual void Start()final;
	virtual void AbsorbPhoton(Photon&photon,RANDOM&R)final;
	virtual void End()final;
	virtual RectDimensions&&Dimensions()final;
	virtual double GlueEfficiency()const final;
	PhotoSensitiveSurface&operator>>(std::shared_ptr<PhotonTimeAcceptor>);
protected:
	//Photons come in not sorted by time, but are sorted here
	virtual void PhotonTimeAccepted(double time,RANDOM&R);
private:
	double m_glue;
	Func m_efficiency;//depends on lambda
	std::vector<std::shared_ptr<PhotonTimeAcceptor>> m_signal;
	std::uniform_real_distribution<double> P;
	Vec times;
};
//There's a problem with transfering vector<smth>&& parameter as {val1,val2,...} to make_Shared template function
inline std::shared_ptr<PhotoSensitiveSurface> Photosensor(std::vector<Pair>&&dimensions,double glue,Func efficiency){
	return std::shared_ptr<PhotoSensitiveSurface>(new PhotoSensitiveSurface(static_cast<std::vector<Pair>&&>(dimensions),glue,efficiency));
}
class PhotoSensitiveSurfaceWithTTS:public PhotoSensitiveSurface{
public:
    PhotoSensitiveSurfaceWithTTS(std::vector<Pair>&& dimensions, double glue, Func efficiency,double tts);
    virtual ~PhotoSensitiveSurfaceWithTTS();
protected:
    virtual void PhotonTimeAccepted(double time, RANDOM& R)override;
private:
	std::normal_distribution<double> m_tts;;
};
//There's a problem with transfering vector<smth>&& parameter as {val1,val2,...} to make_Shared template function
inline std::shared_ptr<PhotoSensitiveSurface> Photosensor(std::vector<Pair>&&dimensions,double glue,Func efficiency,double tts){
	return std::shared_ptr<PhotoSensitiveSurfaceWithTTS>(new PhotoSensitiveSurfaceWithTTS(static_cast<std::vector<Pair>&&>(dimensions),glue,efficiency,tts));
}
#endif