#ifndef XSAYaAus
#define XSAYaAus
#include "rectscin.h"
class FlatLightguide:public virtual IPhotonAbsorber,protected virtual ScintillatorSurface{
public:
	FlatLightguide(std::vector<Pair>&&dimensions,double glue_eff,double n,double H);
	virtual ~FlatLightguide();
	FlatLightguide&operator>>(std::shared_ptr<IPhotonAbsorber>sensor);
	//IPhotoSensitive
    virtual void Start()override;
    virtual void AbsorbPhoton(Photon& photon)override;
    virtual void End()override;
    virtual double GlueEfficiency()override;
    virtual RectDimensions&& Dimensions()override;
private:
	double g_eff,refr,height;
};
//There's a problem with transfering vector<smth>&& parameter as {val1,val2,...} to make_Shared template function
inline std::shared_ptr<FlatLightguide> LightGuide(std::vector<Pair>&&dimensions,double glue_eff,double n,double H){
	return std::shared_ptr<FlatLightguide>(new FlatLightguide(static_right(dimensions),glue_eff,n,H));
}
#endif