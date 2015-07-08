// this file is distributed under
// GPL v 3.0 license
#include <iostream>
#include <rectscin.h>
#include <sensitive.h>
#include <signal.h>
using namespace std;
int main(int , char **){
	RectangularScintillator scintillator(
		{make_pair(0,30),make_pair(0,14),make_pair(0,14)},
		TimeDistribution2(0.005,0.2,1.5),
		RandomValueGenerator<double>([](double l){return 1.0;},360,480,120),
		[](double l){return 1.58;},
		[](double l){return 0.08;}
	);
	auto leftcnt=make_shared<Counter>();
	auto rightcnt=make_shared<Counter>();
	scintillator.Surface(0,RectDimensions::IntersectionSearchResults::Left)<<(
		Photosensor({make_pair(0,14),make_pair(0,14)},[](double l){return 0.3;},0.68)<<leftcnt
	);
	scintillator.Surface(0,RectDimensions::IntersectionSearchResults::Right)<<(
		Photosensor({make_pair(0,14),make_pair(0,14)},[](double l){return 0.3;},0.68)<<rightcnt
	);
	//for(unsigned int cnt=0;cnt<5000;cnt++)
		scintillator.RegisterGamma({15,7,7},3000);
	//printf("Photons number left: %f +/- %f \n",leftcnt->average(),leftcnt->sigma());
	//printf("Photons number right: %f +/- %f \n",rightcnt->average(),rightcnt->sigma());
	//printf("Time resolution left: %f [ns] \n",0);
	//printf("Time resolution right: %f [ns] \n",0);
	return 0;
}
