// this file is distributed under
// GPL v 3.0 license
#include <iostream>
#include <math_h/gnuplot/gnuplot.h>
#include <rectscin.h>
#include <lightguide.h>
#include <sensitive.h>
#include <signal_model.h>
using namespace std;
int main(int , char **){
	auto efficiency=[](double l){return 0.3;};
	auto absorption=[](double l){return 0.08;};
	double refraction=1.58;
	RectangularScintillator scintillator(
		{make_pair(-15,15),make_pair(-7,7),make_pair(-7,7)},
		TimeDistribution2(0.005,0.2,1.5),
		RandomValueGenerator<double>([](double l){return 1.0;},360,480,2),
		refraction,absorption
	);
	auto leftcnt=make_shared<Counter>();
	auto rightcnt=make_shared<Counter>();
	auto lefttime=make_shared<Timer>(0);
	auto righttime=make_shared<Timer>(0);
	auto leftdistr=make_shared<TimeDistribution>(-2,10,120);
	auto rightdistr=make_shared<TimeDistribution>(-2,10,120);
	scintillator.Surface(0,RectDimensions::Left)<<(
		LightGuide({make_pair(-7,7),make_pair(-7,7)},0,1,10)<<(
			Photosensor({make_pair(-7,7),make_pair(-7,7)},efficiency,0.68)<<leftcnt<<lefttime<<leftdistr
		)
	);
	scintillator.Surface(0,RectDimensions::Right)<<(
		Photosensor({make_pair(-7,7),make_pair(-7,7)},efficiency,0.68)<<rightcnt<<righttime<<rightdistr
	);
	for(unsigned int cnt=0;cnt<1000;cnt++)
		scintillator.RegisterGamma({0,0,0},3000);
	printf("Photons number left: %f+/-%f\n",leftcnt->average(),leftcnt->sigma());
	printf("Photons number right: %f+/-%f\n",rightcnt->average(),rightcnt->sigma());
	printf("Time left: %f+/-%f\n",lefttime->average(),lefttime->sigma());
	printf("Time right: %f+/-%f\n",righttime->average(),righttime->sigma());
	Plotter::Instance().SetOutput(".");
	Plot<double>().Hist("left",leftdistr->GetDistribution()).Hist("right",rightdistr->GetDistribution());
	return 0;
}
