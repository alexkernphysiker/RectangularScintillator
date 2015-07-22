// this file is distributed under
// GPL v 3.0 license
#include <iostream>
#include <rectscin.h>
#include <sensitive.h>
#include <photon_accept.h>
using namespace std;
int main(int , char **){
	RectangularScintillator scintillator(
		{make_pair(-15,15),make_pair(-7,7),make_pair(-7,7)},
		TimeDistribution2(0.005,0.2,1.5),
		RandomValueGenerator<double>([](double l){return 1.0;},360,480,2),
		1.58,[](double l){return 0.08;}
	);
	auto Phm=[](){return Photosensor({make_pair(-7,7),make_pair(-7,7)},1,[](double l){return 0.3;});};
	auto left_order=make_shared<OrderStatistics>(3);
	{
		auto lphm=Phm(),rphm=Phm();
		scintillator.Surface(0,RectDimensions::Left)>>lphm;
		scintillator.Surface(0,RectDimensions::Right)>>rphm;
		lphm>>left_order;
	}
	printf("Simulation...\n");
	for(unsigned int cnt=0;cnt<500;cnt++)scintillator.RegisterGamma({0,0,0},3000);
	printf("done.\n");
	for(unsigned int i=0,n=left_order->count();i<n;i++)
		printf("PhotonTime(%i): %f+/-%f\n",i,left_order->At(i).getAverage(),left_order->At(i).getSigma());
	
	return 0;
}
