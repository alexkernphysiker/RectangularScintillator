// this file is distributed under 
// MIT license
#include <math_h/functions.h>
#include "test_objects.h"
using namespace std;
using namespace MathTemplates;
using namespace RectangularScintillator;
TEST(Scintillator,reflection){
	Scintillator rsc({make_pair(0,1),make_pair(0,1),make_pair(0,1)},1.0+0.1*(rand()%10),TimeDistribution1(0.5,1.5));
	for(Pair&p:rsc.ReflectionProbabilityFunction())
		EXPECT_TRUE((p.second>=0)&&(p.second<=1));
}
TEST(Scintillator,geometry){
	Scintillator rsc({make_pair(0,1),make_pair(0,1),make_pair(0,1)},1.0,TimeDistribution1(0.5,1.5));
	EXPECT_NO_THROW(rsc.RegisterGamma({0.5,0.5,0.5},10,engine));
	EXPECT_THROW(rsc.RegisterGamma({2,2,2},1,engine),Exception<Scintillator>);
}
TEST(Scintillator, Isotropic){
	Scintillator rsc({make_pair(0,1),make_pair(0,1),make_pair(0,1)},1.6,TimeDistribution1(0.5,1.5));
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
		rsc.RegisterGamma({0.5,0.5,0.5},3000,engine);
	auto check_close=[](vector<shared_ptr<SignalStatictics>>&vec){
		double val=INFINITY,err;
		for(shared_ptr<SignalStatictics>one:vec)
			if(isfinite(val)){
				EXPECT_CLOSE_VALUES_with_error(one->data().get().val(),val,err+one->data().get().val());
			}else{
				val=one->data().get().val();
				err=one->data().get().delta();
			}
	};
	check_close(counts);
	check_close(times);
}
TEST(Scintillator, Isotropic2){
	Scintillator rsc({make_pair(0,1),make_pair(0,1),make_pair(0,1)},1.6,TimeDistribution1(0.5,1.5));
	vector<shared_ptr<SignalStatictics>> counts,times;
	for(size_t dimension=0;dimension<3;dimension++)
		for(auto side=RectDimensions::Left;side<=RectDimensions::Right;inc(side))
			for(double x1=-1;x1<1;x1+=1)
				for(double x2=-1;x2<1;x2+=1){
					auto ampl=make_shared<SignalStatictics>(2),time=make_shared<SignalStatictics>(2);
					rsc.Surface(dimension,side)>>(
						Photosensor({make_pair(x1,x1+1),make_pair(x2,x2+1)},1.0,[](double){return 1.0;})
							>>(make_shared<AmplitudeSignal>()>>ampl)
							>>(TimeSignal({make_pair(0,1)})>>time)
					);
					counts.push_back(ampl);
					times.push_back(time);
				}
	for(size_t cnt=0;cnt<400;cnt++)
		rsc.RegisterGamma({0,0,0},3000,engine);
	auto check_close=[](vector<shared_ptr<SignalStatictics>>&vec){
		value<double> val=vec[0]->data().get();
		for(shared_ptr<SignalStatictics>one:vec)
			EXPECT_TRUE(val.contains(one->data().get()));
	};
	check_close(counts);
	check_close(times);
}
TEST(Scintillator,Glue){
	Scintillator rsc({make_pair(-5,5),make_pair(-5,5),make_pair(-5,5)},1.6,TimeDistribution1(0.5,1.5));
	auto ideal=make_shared<SignalStatictics>(),
		middle=make_shared<SignalStatictics>(),
		worst=make_shared<SignalStatictics>();
	rsc.Surface(0,RectDimensions::Left)>>(
		Photosensor({make_pair(-5,5),make_pair(-5,5)},1.0,[](double){return 1.0;})
		>>(make_shared<AmplitudeSignal>()>>ideal)
	);
	rsc.Surface(1,RectDimensions::Left)>>(
		Photosensor({make_pair(-5,5),make_pair(-5,5)},0.5,[](double){return 1.0;})
		>>(make_shared<AmplitudeSignal>()>>middle)
	);
	rsc.Surface(2,RectDimensions::Left)>>(
		Photosensor({make_pair(-5,5),make_pair(-5,5)},0.0,[](double){return 1.0;})
		>>(make_shared<AmplitudeSignal>()>>worst)
	);
	for(size_t cnt=0;cnt<200;cnt++)
		rsc.RegisterGamma({0,0,0},3000,engine);
	EXPECT_TRUE(worst->data().get().val()<ideal->data().get().val());
	EXPECT_TRUE(worst->data().get().val()<middle->data().get().val());
	EXPECT_TRUE(middle->data().get().val()<ideal->data().get().val());
}
TEST(Scintillator, oneD_symmetry_plus_concurrency){
	vector<shared_ptr<Scintillator>> rsc{
		MakeScintillator({make_pair(-50,50),make_pair(-5,5),make_pair(-5,5)},1.6,TimeDistribution1(0.5,1.5)),
		MakeScintillator({make_pair(-50,50),make_pair(-5,5),make_pair(-5,5)},1.6,TimeDistribution1(0.5,1.5))
	};
	vector<shared_ptr<SignalStatictics>>
		timestat{make_shared<SignalStatictics>(2),make_shared<SignalStatictics>(2)},
		amplstat{make_shared<SignalStatictics>(2),make_shared<SignalStatictics>(2)};
	for(size_t index=0;index<2;index++){
		auto timediff=make_shared<SignalSumm>(),ampldiff=make_shared<SignalSumm>();
		{
			auto time=make_shared<Signal>(),ampl=make_shared<Signal>();
			rsc[index]->Surface(0,RectDimensions::Left)>>(
				Photosensor({make_pair(0,1),make_pair(0,1)},1.0,[](double){return 1.0;})
					>>(make_shared<AmplitudeSignal>()>>ampl)
					>>(TimeSignal({make_pair(0,1)})>>time)
			);
			timediff<<time;ampldiff<<ampl;
		}
		{
			auto time=make_shared<Signal>(),ampl=make_shared<Signal>();
			rsc[index]->Surface(0,RectDimensions::Right)>>(
				Photosensor({make_pair(0,1),make_pair(0,1)},1.0,[](double){return 1.0;})
					>>(make_shared<AmplitudeSignal>()>>(SignalInvert()>>ampl))
					>>(TimeSignal({make_pair(0,1)})>>(SignalInvert()>>time))
			);
			timediff<<time;ampldiff<<ampl;
		}
		timediff>>(timestat[index]);ampldiff>>(amplstat[index]);
	}
	for(size_t threads=1;threads<3;threads++){
		for(size_t index=0;index<2;index++)rsc[index]->Configure(Scintillator::Options(threads,5));
		cout<< threads<<" threads"<<endl;
		for(size_t cnt=0;cnt<200;cnt++)
			rsc[0]->RegisterGamma({-30,0,0},3000,engine);
		for(size_t cnt=0;cnt<200;cnt++)
			rsc[1]->RegisterGamma({+30,0,0},3000,engine);
		EXPECT_TRUE(amplstat[0]->data().get().contains(amplstat[1]->data().get()));
		EXPECT_TRUE(timestat[0]->data().get().contains(timestat[1]->data().get()));
	}
}
