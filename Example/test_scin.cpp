// this file is distributed under
// GPL v 3.0 license
#include <iostream>
#include <rectscin.h>
#include <sensitive.h>
#include <signal_model.h>
using namespace std;
int main(int , char **){
	RectangularScintillator scintillator(
		{make_pair(-15,15),make_pair(-7,7),make_pair(-7,7)},
		TimeDistribution2(0.005,0.2,1.5),
		RandomValueGenerator<double>([](double l){return 1.0;},360,480,2),
		1.58,[](double l){return 0.08;}
	);
	auto efficiency=[](double l){return 0.3;};
	auto leftcnt=make_shared<Counter>();
	auto rightcnt=make_shared<Counter>();
	scintillator.Surface(0,RectDimensions::Left).Glue({make_pair(-7,7),make_pair(-7,7)})
		<<(Photosensor({make_pair(-7,7),make_pair(-7,7)},efficiency)<<leftcnt);
	scintillator.Surface(0,RectDimensions::Right).Glue({make_pair(-7,7),make_pair(-7,7)})
		<<(Photosensor({make_pair(-7,7),make_pair(-7,7)},efficiency)<<rightcnt);
	for(unsigned int cnt=0;cnt<1000;cnt++)
		scintillator.RegisterGamma({0,0,0},3000);
	printf("Photons number left: %f+/-%f\n",leftcnt->average(),leftcnt->sigma());
	printf("Photons number right: %f+/-%f\n",rightcnt->average(),rightcnt->sigma());
	return 0;
}
