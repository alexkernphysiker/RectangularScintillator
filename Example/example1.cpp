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
		TimeDistribution2(0.005,0.2,1.5),
		RandomValueGenerator<double>([](double l){return 1.0;},360,480,2),
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
			auto s_left=make_shared<WeightedTimeSignal>();
			s_left->AddSummand(i,1);
			lphm>>(s_left>>left_time[i]);
			auto s_right=make_shared<WeightedTimeSignal>();
			s_right->AddSummand(i,1);
			rphm>>s_right;
			auto diff=(SignalSum({1,-1})<<s_left<<s_right)>>diff_time[i];
			if(i==0)diff>>diff_distr;
		}
	}
	printf("Simulation...\n");
	for(unsigned int cnt=0;cnt<500;cnt++)scintillator.RegisterGamma({0,0,0},3000);
	printf("done.\n");
	for(unsigned int i=0,n=left_time.size();i<n;i++)
		printf("Photoelectron Time(%i): %f+/-%f [%i]\n",i,left_time[i]->getAverage(),left_time[i]->getSigma(),left_time[i]->count());
	for(unsigned int i=0,n=diff_time.size();i<n;i++)
		printf("Time Difference(%i): %f+/-%f [%i]\n",i,diff_time[i]->getAverage(),diff_time[i]->getSigma(),diff_time[i]->count());
	printf("Photoelectron count: %f+/-%f [%i]\n",left_count->getAverage(),left_count->getSigma(),left_count->count());
	printf("=================================\n");
	for(size_t i=0,n=diff_distr->size();i<n;i++)
		printf("[%f > %f\n",diff_distr->getX(i),diff_distr->getY(i));
	return 0;
}
