// this file is distributed under
// GPL v 3.0 license
#include <iostream>
#include <rectscin.h>
#include <sensitive.h>
#include <photon2signal.h>
#include <signal_statistics.h>
using namespace std;
int main(int , char **){
	RectangularScintillator scintillator(
		{make_pair(-15,15),make_pair(-7,7),make_pair(-7,7)},
		TimeDistribution2(0.005,0.2,1.5),
		RandomValueGenerator<double>([](double l){return 1.0;},360,480,2),
		1.58,[](double l){return 0.08;}
	);
	auto Phm=[](){return Photosensor({make_pair(-7,7),make_pair(-7,7)},1,[](double l){return 0.3;});};
	vector<shared_ptr<TimeSignalStatictics>> left;
	{
		auto lphm=Phm(),rphm=Phm();
		scintillator.Surface(0,RectDimensions::Left)>>lphm;
		scintillator.Surface(0,RectDimensions::Right)>>rphm;
		for(size_t i=0;i<10;i++){
			left.push_back(make_shared<TimeSignalStatictics>());
			auto signal=make_shared<WeightedTimeSignal>();
			signal->AddSummand(i,1);
			lphm>>dynamic_pointer_cast<PhotonTimeAcceptor>(signal>>left[i]);
		}
	}
	printf("Simulation...\n");
	for(unsigned int cnt=0;cnt<500;cnt++)scintillator.RegisterGamma({0,0,0},3000);
	printf("done.\n");
	for(unsigned int i=0,n=left.size();i<n;i++)
		printf("PhotonTime(%i): %f+/-%f [%i]\n",i,left[i]->getAverage(),left[i]->getSigma(),left[i]->count());
	
	return 0;
}
