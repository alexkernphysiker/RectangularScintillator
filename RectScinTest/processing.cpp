// this file is distributed under 
// GPL v 3.0 license
#include "test_objects.h"
TEST(SignalPolinomialDistort,Base){
	for(size_t p=0;p<10;p++){
		Vec C;for(size_t i=0;i<=p;i++)C.push_back(rand());
		auto test=Distortion(static_right(C));
		SignalSender sender;auto out=make_shared<Out>();
		sender>>(test>>out);
		double signal=rand();
		sender.send({signal});
		EXPECT_CLOSE_VALUES(out->value(),Polynom(signal,C,p));
	}
}