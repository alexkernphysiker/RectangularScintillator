// this file is distributed under 
// MIT license
#ifndef XSAYaAus
#define XSAYaAus
#include "scintillator.h"
namespace RectangularScintillator{
	using namespace std;
	using namespace MathTemplates;
	class FlatLightguide:public virtual IPhotonAbsorber,protected virtual ScintillatorSurface{
	public:
		FlatLightguide(vector<Pair>&&dimensions,double glue_eff,double n,double H);
		virtual ~FlatLightguide();
		FlatLightguide&operator>>(shared_ptr<IPhotonAbsorber>sensor);
		//IPhotoSensitive
		virtual void Start()override;
		virtual void AbsorbPhoton(Photon& photon,RANDOM&R)override;
		virtual void End()override;
		virtual double GlueEfficiency()const override;
		virtual const RectDimensions&Dimensions()const override;
	private:
		
		double g_eff,refr,height;
	};
	//There's a problem with transfering vector<smth>&& parameter as {val1,val2,...} to make_Shared template function
	inline shared_ptr<FlatLightguide> LightGuide(vector<Pair>&&dimensions,double glue_eff,double n,double H){
		return shared_ptr<FlatLightguide>(new FlatLightguide(static_cast<vector<Pair>&&>(dimensions),glue_eff,n,H));
	}
};
#endif