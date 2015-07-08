// this file is distributed under
// GPL v 3.0 license
#include <iostream>
#include <rectscin.h>
#include <sensitive.h>
#include <signal_model.h>
using namespace std;
int main(int , char **){
	RectangularScintillator scintillator(
		{make_pair(0,30),make_pair(-7,7),make_pair(-7,7)},
		TimeDistribution2(0.005,0.2,1.5),
		RandomValueGenerator<double>([](double l){return 1.0;},360,480,2),
		[](double l){return 1.58;},
		[](double l){return 0.08;}
	);
	auto efficiency=[](double l){return 0.3;};
	auto leftcnt=make_shared<Counter>();
	auto rightcnt=make_shared<Counter>();
	auto lefttime=make_shared<Timer>(0);
	auto righttime=make_shared<Timer>(0);
	scintillator.Surface(0,RectDimensions::IntersectionSearchResults::Left)<<(
		Photosensor({make_pair(-7,7),make_pair(-7,7)},efficiency)<<leftcnt<<lefttime
	);
	scintillator.Surface(0,RectDimensions::IntersectionSearchResults::Right)<<(
		Photosensor({make_pair(-7,7),make_pair(-7,7)},efficiency)<<rightcnt<<righttime
	);
	for(unsigned int cnt=0;cnt<50;cnt++)
		scintillator.RegisterGamma({15,0,0},3000);
	printf("Photons number left: %f +/- %f \n",leftcnt->average(),leftcnt->sigma());
	printf("Photons number right: %f +/- %f \n",rightcnt->average(),rightcnt->sigma());
	printf("Time resolution left: %f [ns] \n",lefttime->sigma());
	printf("Time resolution right: %f [ns] \n",righttime->sigma());
	return 0;
}
