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
class PhotoSensitiveSurface:public IPhotoSensitive,protected RectDimensions{
public:
	PhotoSensitiveSurface(std::vector<Pair>&&dimensions,double glue,Func efficiency);
	virtual ~PhotoSensitiveSurface();
	virtual void Start()override;
	virtual void RegisterPhoton(Photon&photon)override;
	virtual void End()override;
    virtual RectDimensions&& Dimensions()override;
    virtual double GlueEfficiency()override;
	PhotoSensitiveSurface&operator>>(std::shared_ptr<PhotonTimeAcceptor>);
protected:
	virtual void PhotonTimeAccepted(double time);
	std::default_random_engine rand;
private:
	double m_glue;
	Func m_efficiency;//depends on lambda
	std::vector<std::shared_ptr<PhotonTimeAcceptor>> m_signal;
	std::uniform_real_distribution<double> P;
	Vec times;
};
class PhotoSensitiveSurfaceWithTTS:public PhotoSensitiveSurface{
public:
    PhotoSensitiveSurfaceWithTTS(std::vector< Pair >&& dimensions,double glue,Func efficiency, double tts);
    virtual ~PhotoSensitiveSurfaceWithTTS();
protected:
	virtual void PhotonTimeAccepted(double time)override;
private:
	std::normal_distribution<double> m_tts;
};
std::shared_ptr<PhotoSensitiveSurface> operator>>(std::shared_ptr<PhotoSensitiveSurface>,std::shared_ptr<PhotonTimeAcceptor>);
std::shared_ptr<PhotoSensitiveSurface> Photosensor(std::vector<Pair>&&dimensions,double glue,Func efficiency);
std::shared_ptr<PhotoSensitiveSurfaceWithTTS> Photosensor(std::vector<Pair>&&dimensions,double glue,Func efficiency,double tts);
#endif