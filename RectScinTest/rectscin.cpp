// this file is distributed under 
// GPL v 3.0 license
#include <math_h/functions.h>
#include "test_objects.h"
TEST(RectangularScintillator,reflection){
	RectangularScintillator rsc(
		{make_pair(0,1),make_pair(0,1),make_pair(0,1)},
		TimeDistribution1(0.5,1.5),RandomValueGenerator<double>(100,200),
		1.0+0.1*(rand()%10),[](double){return 0.0;}
	);
	for(Pair&p:rsc.ReflectionProbabilityFunction())
		EXPECT_TRUE((p.second>=0)&&(p.second<=1));
}
TEST(RectangularScintillator,geometry){
	RectangularScintillator rsc(
		{make_pair(0,1),make_pair(0,1),make_pair(0,1)},
		TimeDistribution1(0.5,1.5),RandomValueGenerator<double>(100,200),
		1,[](double){return 0.0;}
	);
	EXPECT_NO_THROW(rsc.RegisterGamma({0.5,0.5,0.5},10));
	EXPECT_THROW(rsc.RegisterGamma({2,2,2},1),RectScinException);
}
TEST(RectangularScintillator, Isotropic){
	RectangularScintillator rsc(
		{make_pair(0,1),make_pair(0,1),make_pair(0,1)},
		TimeDistribution1(0.5,1.5),RandomValueGenerator<double>(100,200),
		1.6,[](double){return 0.0;}
	);
	vector<shared_ptr<SignalStatictics>> counts,times;
	for(size_t dimension=0;dimension<3;dimension++)
		for(auto side=RectDimensions::Left;side<=RectDimensions::Right;inc(side)){
			auto ampl=make_shared<SignalStatictics>(),time=make_shared<SignalStatictics>();
			rsc.Surface(dimension,side)>>(
				Photosensor({make_pair(0,1),make_pair(0,1)},1.0,[](double){return 1.0;})
					>>(make_shared<AmplitudeSignal>()>>ampl)
					>>(TimeSignal({make_pair(0,1)})>>time)
			);
			counts.push_back(ampl);
			times.push_back(time);
		}
	for(size_t cnt=0;cnt<200;cnt++)
		rsc.RegisterGamma({0.5,0.5,0.5},3000);
	auto check_close=[](vector<shared_ptr<SignalStatictics>>&vec){
		double val=INFINITY,err;
		for(shared_ptr<SignalStatictics>one:vec)
			if(isfinite(val)){
				EXPECT_TRUE(pow(val-one->data().getAverage(),2)<=pow(err+one->data().getSigma(),2));
			}else{
				val=one->data().getAverage();
				err=one->data().getSigma();
			}
	};
	check_close(counts);
	check_close(times);
}