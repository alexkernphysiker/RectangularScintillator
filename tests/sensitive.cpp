// this file is distributed under 
// MIT license
#include <math_h/functions.h>
#include "test_objects.h"
using namespace std;
using namespace MathTemplates;
using namespace RectangularScintillator;
class PhotonCheck:public PhotonTimeAcceptor{
private:
	Vec times;
public:
	PhotonCheck(){}
	virtual ~PhotonCheck(){}
	Vec& data()const{return const_cast<Vec&>(times);}
	virtual void AcceptEventStart()override{times.clear();}
	virtual void AcceptPhotonTime(double time)override{times.push_back(time);}
	virtual void AcceptEventEnd()override{}
};
TEST(PhotoSensitiveSurface,Glue_eff){
	for(double glue=0;glue<=1;glue+=0.1){
		auto Phm=Photosensor({make_pair(-1,1),make_pair(-1,1)},glue,[](double){return 1;});
		EXPECT_EQ(glue,Phm->GlueEfficiency());
		EXPECT_EQ(2,Phm->Dimensions().NumberOfDimensions());
	}
}
TEST(PhotoSensitiveSurface,Efficiency){
	for(double eff=0;eff<=1;eff+=1){
		auto Phm=Photosensor({make_pair(-1,1),make_pair(-1,1)},1,[eff](double){return eff;});
		auto check=make_shared<PhotonCheck>();
		Phm>>check;
		Sigma<double> stat;
		for(size_t cnt=0;cnt<1000;cnt++){
			Phm->Start();
			for(size_t i=0;i<1000;i++){
				Photon ph;
				ph.coord={0,0};
				ph.dir={0,0};
				ph.lambda=1;
				ph.time=Rand(engine);
				Phm->AbsorbPhoton(ph,engine);
			}
			Phm->End();
			stat.AddValue(check->data().size());
		}
		if((eff==0)||(eff==1))
			EXPECT_EQ(eff*1000,stat.getAverage());
		EXPECT_CLOSE_VALUES_with_error(eff*1000,stat.getAverage(),stat.getSigma()*2);
	}
}
TEST(PhotoSensitiveSurface,Geometry){
	auto Phm=Photosensor({make_pair(-1,1),make_pair(-1,1)},1,[](double){return 1;});
	auto check=make_shared<PhotonCheck>();
	Phm>>check;
	for(size_t n=0;n<100;n++)
		for(size_t cnt=0;cnt<10;cnt++){
			Phm->Start();
			for(size_t i=0;i<n;i++){
				Photon ph;
				ph.coord={2,2};
				ph.dir={0,0};
				ph.lambda=1;
				ph.time=Rand(engine);
				Phm->AbsorbPhoton(ph,engine);
			}
			Phm->End();
			EXPECT_EQ(0,check->data().size());
			Phm->Start();
			for(size_t i=0;i<n;i++){
				Photon ph;
				ph.coord={0,0};
				ph.dir={0,0};
				ph.lambda=1;
				ph.time=double(rand()%1000)/100;
				Phm->AbsorbPhoton(ph,engine);
			}
			Phm->End();
			EXPECT_EQ(n,check->data().size());
		}
}
TEST(PhotoSensitiveSurface,PhotonSortingTest){
	auto Phm=Photosensor({make_pair(-1,1),make_pair(-1,1)},1,[](double){return 1;});
	auto check=make_shared<PhotonCheck>();
	Phm>>check;
	for(size_t n=0;n<100;n++)
		for(size_t cnt=0;cnt<10;cnt++){
			Phm->Start();
			for(size_t i=0;i<n;i++){
				Photon ph;
				ph.coord={0,0};
				ph.dir={0,0};
				ph.lambda=1;
				ph.time=Rand(engine);
				Phm->AbsorbPhoton(ph,engine);
			}
			Phm->End();
			for(size_t i=1;i<n;i++)
				EXPECT_TRUE(check->data()[i-1]<=check->data()[i]);
		}
}
TEST(PhotoSensitiveSurfaceWithTTS,tts){
	for(double tts=0;tts<0.6;tts+=0.1){
		auto Phm=Photosensor({make_pair(-1,1),make_pair(-1,1)},1,[](double){return 1;},tts);
		auto check=make_shared<PhotonCheck>();
		Phm>>check;
		Sigma<double> sig;
		for(size_t cnt=0;cnt<1000;cnt++){
			Phm->Start();
			Photon ph;
			ph.coord={0,0};
			ph.dir={0,0};
			ph.lambda=1;
			ph.time=1;
			Phm->AbsorbPhoton(ph,engine);
			Phm->End();
			sig.AddValue(check->data()[0]);
		}
		EXPECT_CLOSE_VALUES_with_error(tts,sig.getSigma(),tts*0.2);
	}
}
