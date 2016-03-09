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
		FlatLightguide(const vector<Pair>&dimensions,const double glue_eff,const double n,const double H);
		virtual ~FlatLightguide();
		FlatLightguide&operator>>(const shared_ptr<IPhotonAbsorber>sensor);
		virtual void Start()override;
		virtual void AbsorbPhoton(Photon& photon,RANDOM&R)override;
		virtual void End()override;
		virtual double GlueEfficiency()const override;
		virtual const RectDimensions&Dimensions()const override;
	private:
		
		double g_eff,refr,height;
	};
	inline const shared_ptr<FlatLightguide> LightGuide(const vector<Pair>&&dimensions,const double glue_eff,const double n,const double H){
		return shared_ptr<FlatLightguide>(new FlatLightguide(dimensions,glue_eff,n,H));
	}
};
#endif