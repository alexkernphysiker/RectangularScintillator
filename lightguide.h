#ifndef XSAYaAus
#define XSAYaAus
#include "rectscin.h"
class FlatLightguide:public virtual IPhotoSensitive,protected virtual ScintillatorSurface{
public:
	FlatLightguide(std::vector<Pair>&&dimensions,double glue_eff,double n,double H);
	virtual ~FlatLightguide();
	FlatLightguide&operator>>(std::shared_ptr<IPhotoSensitive>sensor);
	//IPhotoSensitive
    virtual void Start()override;
    virtual void RegisterPhoton(Photon& photon)override;
    virtual void End()override;
    virtual double GlueEfficiency()override;
    virtual RectDimensions&& Dimensions()override;
private:
	double g_eff,refr,height;
};
std::shared_ptr<FlatLightguide> operator>>(std::shared_ptr<FlatLightguide>,std::shared_ptr<IPhotoSensitive>sensor);
std::shared_ptr<FlatLightguide> LightGuide(std::vector<Pair>&&dimensions,double glue_eff,double n,double H);
#endif