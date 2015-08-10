// this file is distributed under
// GPL v 3.0 license
#include <iostream>
#include <rectscin.h>
#include <sensitive.h>
#include <photon2signal.h>
#include <signal_processing.h>
#include <signal_statistics.h>
using namespace std;
int main(int , char **){
	RectangularScintillator scintillator(
		{make_pair(-15,15),make_pair(-7,7),make_pair(-7,7)},
		TimeDistribution2(0.005,0.2,1.5),RandomValueGenerator<double>(300,500),
		1.58,[](double l){return 0.08;}
	);
	auto time_difference=make_shared<SignalStatictics>();
	{
		auto photosensor=[](){return Photosensor({make_pair(-7,7),make_pair(-7,7)},1,[](double l){return 0.3;});};
		auto left=make_shared<Signal>(),right=make_shared<Signal>();
		scintillator.Surface(0,RectDimensions::Left)>>(photosensor()>>(TimeSignal({make_pair(0,1)})>>left));
		scintillator.Surface(0,RectDimensions::Right)>>(photosensor()>>(TimeSignal({make_pair(0,1)})>>(SignalInvert()>>right)));
		(make_shared<SignalSumm>()<<left<<right)>>time_difference;
	}
	printf("Simulation...\n");
	RANDOM engine;
	for(unsigned int cnt=0;cnt<500;cnt++)
		scintillator.RegisterGamma({0,0,0},3000,engine);
	printf("done.\n");
	printf("Time difference = %f+/-%f[ns]\n",time_difference->data().getAverage(),time_difference->data().getSigma());
	return 0;
}
