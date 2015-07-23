// this file is distributed under 
// GPL v 3.0 license
#include <math_h/functions.h>
#include "test_objects.h"
TEST(SignalPolinomialDistort,Base){
	for(size_t p=0;p<10;p++){
		Vec C;for(size_t i=0;i<=p;i++)C.push_back(rand());
		auto test=Distortion(static_right(C));
		SignalSender sender;auto out=make_shared<Out>();
		sender>>(test>>out);
		double signal=rand();
		sender.send({signal});
		double expected=Polynom(signal,C,p);
		EXPECT_CLOSE_VALUES(out->value(),expected);
	}
}
TEST(SignalSmear,Base){
	for(double x=-10;x<=10;x+=0.5)
		for(double sig=0;sig<5;sig+=0.5){
			auto test=make_shared<SignalSmear>(sig);
			SignalSender sender;auto out=make_shared<SignalStatictics>();
			sender>>(test>>out);
			for(size_t i=0;i<50000;i++)sender.send({x});
			APROXIMATELY_CLOSE_VALUES(x,out->data().getAverage());
			APROXIMATELY_CLOSE_VALUES(sig,out->data().getSigma());
		}
}
