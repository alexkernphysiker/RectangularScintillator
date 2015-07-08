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
	PhotoSensitiveSurface(std::vector<Pair>&&dimensions,Func efficiency,double tts);
	virtual ~PhotoSensitiveSurface();
	virtual void Start()override;
	virtual void RegisterPhoton(Photon&photon)override;
	virtual void End()override;
	PhotoSensitiveSurface&operator<<(std::shared_ptr<ISignal>);
private:
	std::normal_distribution<double> m_tts;
	Func m_efficiency;//depends on lambda
	std::vector<std::shared_ptr<ISignal>> m_signal;
	std::default_random_engine rand;
};
std::shared_ptr<PhotoSensitiveSurface> operator<<(std::shared_ptr<PhotoSensitiveSurface>,std::shared_ptr<ISignal>);
std::shared_ptr<PhotoSensitiveSurface> Photosensor(std::vector<Pair>&&dimensions,Func efficiency,double tts=0);
#endif