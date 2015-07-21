// this file is distributed under
// GPL v 3.0 license
#include <iostream>
#include <rectscin.h>
#include <sensitive.h>
#include <signal_model.h>
#include <signal_analyse.h>
using namespace std;
auto CreateScin=[](){
	auto res=new RectangularScintillator(
		{make_pair(-15,15),make_pair(-7,7),make_pair(-7,7)},
		TimeDistribution2(0.005,0.2,1.5),
		RandomValueGenerator<double>([](double l){return 1.0;},360,480,2),
		1.58,[](double l){return 0.08;}
	);
	return shared_ptr<RectangularScintillator>(res);
};
auto Phm=[](){return Photosensor({make_pair(-7,7),make_pair(-7,7)},[](double l){return 0.3;});};
auto TimeDifference=[](){
	auto res=make_shared<Scheme<SlotsByNumber,WeightedTimesSum>>();
	res->ConfigureSlotsByNumber({0,1});
	res->WeightedConfigure({make_pair(0,1),make_pair(1,-1)});
	return res;
};
int main(int , char **){
	printf("Preparing...\n");
	Vec weights;{
		{
			auto scintillator=CreateScin();
			auto order=make_shared<OrderStatistics>(5);
			scintillator->Surface(0,RectDimensions::Left)<<(Phm()<<order);
			for(unsigned int cnt=0;cnt<100;cnt++)scintillator->RegisterGamma({0,0,0},3000);
			for(unsigned int i=0,n=order->Count();i<n;i++)
				weights.push_back(order->ByNumber(i).getSigmaSqr());
		}
		for(double&w:weights)
			if((isfinite(w))&&(w>0))
				w=1.0/w;
			else
				w=0;
		double norm=0;
		for(double w:weights)norm+=w;
		for(double&w:weights)w/=norm;
	}
	printf("done.\n");
	auto scintillator=CreateScin();
	auto normal=TimeDifference(),normal2=TimeDifference(),weighted=TimeDifference();{
		auto left_f=make_shared<StartTimeSignal>(0),right_f=make_shared<StartTimeSignal>(0),
			 left_s=make_shared<StartTimeSignal>(1),right_s=make_shared<StartTimeSignal>(1);
		auto left_w =make_shared<WeightedTimeSignal>(static_right(weights)),
			 right_w=make_shared<WeightedTimeSignal>(static_right(weights));
		scintillator->Surface(0,RectDimensions::Left)<<(Phm()<<left_f<<left_s<<left_w);
		scintillator->Surface(0,RectDimensions::Right)<<(Phm()<<right_f<<right_s<<right_w);
		normal<<left_f<<right_f;
		normal2<<left_s<<right_s;
		weighted<<left_w<<right_w;
	}
	printf("Simulation...\n");
	for(unsigned int cnt=0;cnt<1000;cnt++)scintillator->RegisterGamma({0,0,0},3000);
	printf("done.\n");
	printf("TimeDifference (1-st): %f+/-%f\n",normal->Weighted().getAverage(),normal->Weighted().getSigma());
	printf("TimeDifference (2-nd): %f+/-%f\n",normal2->Weighted().getAverage(),normal2->Weighted().getSigma());
	printf("TimeDifference (weighted): %f+/-%f\n",weighted->Weighted().getAverage(),weighted->Weighted().getSigma());
	return 0;
}
