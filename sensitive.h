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
	virtual void AbsorbPhoton(Photon&photon)final;
	virtual void End()final;
    virtual RectDimensions&& Dimensions()final;
    virtual double GlueEfficiency()override;
	PhotoSensitiveSurface&operator>>(std::shared_ptr<PhotonTimeAcceptor>);
protected:
	//Photons come in not sorted by time, but are sorted here
	void PhotonTimeAccepted(double time);
	std::default_random_engine rand;
private:
	double m_glue;
	Func m_efficiency;//depends on lambda
	std::vector<std::shared_ptr<PhotonTimeAcceptor>> m_signal;
	std::uniform_real_distribution<double> P;
	Vec times;
};
//There's a problem with transfering vector<smth>&& parameter as {val1,val2,...} to make_Shared template function
inline std::shared_ptr<PhotoSensitiveSurface> Photosensor(std::vector<Pair>&&dimensions,double glue,Func efficiency){
	return std::shared_ptr<PhotoSensitiveSurface>(new PhotoSensitiveSurface(static_right(dimensions),glue,efficiency));
}
#endif