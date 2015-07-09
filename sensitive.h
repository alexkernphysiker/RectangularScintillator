#ifndef fXIVCFLg
#define fXIVCFLg
#include "rectscin.h"
class ISignal{
public:
	virtual ~ISignal(){}
	virtual void Start()=0;
	virtual void Photon(double time)=0;
	virtual void End()=0;
};
class PhotoSensitiveSurface:public IPhotoSensitive,protected RectDimensions{
public:
	PhotoSensitiveSurface(std::vector<Pair>&&dimensions,Func efficiency);
	virtual ~PhotoSensitiveSurface();
	virtual void Start()override;
	virtual void RegisterPhoton(Photon&photon)override;
	virtual void End()override;
	PhotoSensitiveSurface&operator<<(std::shared_ptr<ISignal>);
protected:
	virtual void PhotonTimeAccepted(double time);
	std::default_random_engine rand;
private:
	Func m_efficiency;//depends on lambda
	std::vector<std::shared_ptr<ISignal>> m_signal;
	std::uniform_real_distribution<double> P;
	Vec times;
};
class PhotoSensitiveSurfaceWithTTS:public PhotoSensitiveSurface{
public:
    PhotoSensitiveSurfaceWithTTS(std::vector< Pair >&& dimensions, Func efficiency, double tts);
    virtual ~PhotoSensitiveSurfaceWithTTS();
protected:
	virtual void PhotonTimeAccepted(double time)override;
private:
	std::normal_distribution<double> m_tts;
};
std::shared_ptr<PhotoSensitiveSurface> operator<<(std::shared_ptr<PhotoSensitiveSurface>,std::shared_ptr<ISignal>);
std::shared_ptr<PhotoSensitiveSurface> Photosensor(std::vector<Pair>&&dimensions,Func efficiency);
std::shared_ptr<PhotoSensitiveSurfaceWithTTS> Photosensor(std::vector<Pair>&&dimensions,Func efficiency,double tts);
#endif