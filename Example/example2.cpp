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
	auto Phm1=[](){return Photosensor({make_pair(-7,0),make_pair(-7,7)},1,[](double l){return 0.3;});};
	auto Phm2=[](){return Photosensor({make_pair(0,7),make_pair(-7,7)},1,[](double l){return 0.3;});};
	auto diff_stat=make_shared<SignalStatictics>();
	auto diff_distr=make_shared<SignalDistribution>(-0.4,0.4,20);
	{
		auto l1=TimeSignal({make_pair(0,1)}),
			l2=TimeSignal({make_pair(0,1)}),
			r1=TimeSignal({make_pair(0,1)}),
			r2=TimeSignal({make_pair(0,1)});
		scintillator.Surface(0,RectDimensions::Left)>>(Phm1()>>l1)>>(Phm2()>>l2);
		scintillator.Surface(0,RectDimensions::Right)>>(Phm1()>>r1)>>(Phm2()>>r2);
		(SignalSum({1,-1})
			<<(make_shared<SignalSortAndSelect>(1)<<l1<<l2)
			<<(make_shared<SignalSortAndSelect>(1)<<r1<<r2)
		)>>diff_stat>>diff_distr;
	}
	printf("Simulation...\n");
	for(unsigned int cnt=0;cnt<500;cnt++)scintillator.RegisterGamma({0,0,0},3000);
	printf("done.\n");
	printf("Time difference: %f+/-%f\n",diff_stat->data().getAverage(),diff_stat->data().getSigma());
	printf("=================================\n");
	for(size_t i=0,n=diff_distr->data().size();i<n;i++)
		printf("[%f > %f\n",diff_distr->data().getX(i),diff_distr->data().getY(i));
	return 0;
}
