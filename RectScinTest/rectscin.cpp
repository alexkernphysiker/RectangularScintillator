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