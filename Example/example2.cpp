// this file is distributed under
// GPL v 3.0 license
#include <iostream>
#include <sstream>
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
	auto Phm=[](){return Photosensor({make_pair(-7,7),make_pair(-7,7)},1,[](double l){return 0.3;});};
	vector<shared_ptr<SignalStatictics>> left_time;
	vector<shared_ptr<SignalStatictics>> diff_time;
	auto left_count=make_shared<SignalStatictics>();
	auto diff_distr=make_shared<SignalDistribution>(-0.4,0.4,20);
	{
		auto lphm=Phm(),rphm=Phm();
		scintillator.Surface(0,RectDimensions::Left)>>(lphm>>(make_shared<AmplitudeSignal>()>>left_count));
		scintillator.Surface(0,RectDimensions::Right)>>rphm;
		for(size_t i=0;i<10;i++){
			left_time.push_back(make_shared<SignalStatictics>());
			diff_time.push_back(make_shared<SignalStatictics>());
			auto left=make_shared<Signal>(),right=make_shared<Signal>();
			lphm>>(TimeSignal({make_pair(i,1)})>>(left>>left_time[i]));
			rphm>>(TimeSignal({make_pair(i,1)})>>(SignalInvert()>>right));
			auto diff=(make_shared<SignalSumm>()<<left<<right)>>diff_time[i];
			if(i==0)diff>>diff_distr;
		}
	}
	printf("Simulation...\n");
	RANDOM engine;
	for(unsigned int cnt=0;cnt<500;cnt++)
		scintillator.RegisterGamma({0,0,0},3000,engine);
	printf("done.\n");
	for(unsigned int i=0,n=left_time.size();i<n;i++)
		printf("Photoelectron Time(%i): %f+/-%f\n",i,left_time[i]->data().getAverage(),left_time[i]->data().getSigma());
	for(unsigned int i=0,n=diff_time.size();i<n;i++)
		printf("Time Difference(%i): %f+/-%f\n",i,diff_time[i]->data().getAverage(),diff_time[i]->data().getSigma());
	printf("Photoelectron count: %f+/-%f\n",left_count->data().getAverage(),left_count->data().getSigma());
	printf("=================================\n");
	for(size_t i=0,n=diff_distr->data().size();i<n;i++)
		printf("[%f > %f\n",diff_distr->data().getX(i),diff_distr->data().getY(i));
	return 0;
}
