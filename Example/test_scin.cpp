// this file is distributed under
// GPL v 3.0 license
#include <iostream>
#include <rectscin.h>
#include <sensitive.h>
#include <signal_model.h>
#include <signal_analyse.h>
using namespace std;
int main(int , char **){
	RectangularScintillator scintillator(
		{make_pair(-15,15),make_pair(-7,7),make_pair(-7,7)},
		TimeDistribution2(0.005,0.2,1.5),
		RandomValueGenerator<double>([](double l){return 1.0;},360,480,2),
		1.58,[](double l){return 0.08;}
	);

	auto efficiency=[](double l){return 0.3;};
	auto leftcnt=make_shared<Counter>(),rightcnt=make_shared<Counter>();
	auto lefttime=make_shared<TimeSignal>(),righttime=make_shared<TimeSignal>();
	scintillator.Surface(0,RectDimensions::Left)
		<<(Photosensor({make_pair(-7,7),make_pair(-7,7)},efficiency)<<leftcnt<<lefttime);
	scintillator.Surface(0,RectDimensions::Right)
		<<(Photosensor({make_pair(-7,7),make_pair(-7,7)},efficiency)<<rightcnt<<righttime);
	Scheme<SlotsByNumber,WeightedTimesSum> TimeDifference,TimeLeft;
	TimeDifference.ConfigureSlotsByNumber({0,1});
	TimeDifference.WeightedConfigure({make_pair(0,1),make_pair(1,-1)});
	TimeDifference<<lefttime<<righttime;
	TimeLeft.ConfigureSlotsByNumber({0});
	TimeLeft.WeightedConfigure({make_pair(0,1)});
	TimeLeft<<lefttime;
	
	for(unsigned int cnt=0;cnt<500;cnt++)
		scintillator.RegisterGamma({0,0,0},3000);
	
	printf("Photons number left: %f+/-%f\n",leftcnt->average(),leftcnt->sigma());
	printf("Photons number right: %f+/-%f\n",rightcnt->average(),rightcnt->sigma());
	printf("Time left: %f+/-%f\n",TimeLeft.Weighted().getAverage(),TimeLeft.Weighted().getSigma());
	printf("Time difference: %f+/-%f\n",TimeDifference.Weighted().getAverage(),TimeDifference.Weighted().getSigma());
	return 0;
}
