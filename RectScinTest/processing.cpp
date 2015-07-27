// this file is distributed under 
// GPL v 3.0 license
#include <math_h/functions.h>
#include "test_objects.h"
TEST(SignalPolinomialDistort,Base){
	for(size_t p=0;p<10;p++){
		Vec C;for(size_t i=0;i<=p;i++)C.push_back(rand());
		auto test=PolynomDistort(static_right(C));
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
TEST(SumWithWeights,Base){
	for(size_t n=1;n<=10;n++){
		Vec coefs,signals;
		double expected=0;
		for(size_t i=0;i<n;i++){
			double c=rand(),s=rand();
			coefs.push_back(c);
			signals.push_back(s);
			expected+=c*s;
		}
		auto test=SignalSum(static_right(coefs));
		SignalSender sender;auto out=make_shared<Out>();
		test>>out;
		sender.Connect2MultiInput(test,n);
		sender.send(static_right(signals));
		EXPECT_CLOSE_VALUES(out->value(),expected);
	}
}
TEST(ProductWithPowers,Base){
	for(size_t n=1;n<=10;n++){
		Vec coefs,signals;
		double expected=1;
		for(size_t i=0;i<n;i++){
			double c=rand()%5+1,s=rand()%50;
			coefs.push_back(c);
			signals.push_back(s);
			expected*=pow(s,c);
		}
		auto test=SignalProduct(static_right(coefs));
		SignalSender sender;auto out=make_shared<Out>();
		test>>out;
		sender.Connect2MultiInput(test,n);
		sender.send(static_right(signals));
		EXPECT_CLOSE_VALUES(out->value(),expected);
	}
}
TEST(SignalSortAndSelect,BaseTest){
	for(size_t n=1;n<=10;n++)
		for(size_t orderstatistics=0;orderstatistics<n+5;orderstatistics++){
			auto test=make_shared<SignalSortAndSelect>(orderstatistics);
			SignalSender sender;auto out=make_shared<Out>();
			test>>out; sender.Connect2MultiInput(test,n);
			for(size_t k=0;k<50;k++){
				Vec signals;
				for(double v=0;v<n;v++)
					if(signals.size()==0)
						signals.push_back(v);
					else
						signals.insert(signals.begin()+rand()%(signals.size()+1),v);
				if(orderstatistics<n)
					EXPECT_NO_THROW(sender.send(static_right(signals)));
				else
					EXPECT_THROW(sender.send(static_right(signals)),RectScinException);
				if(orderstatistics<n)
					EXPECT_EQ(orderstatistics,out->value());
				else
					EXPECT_FALSE(isfinite(out->value()));
			}
		}
}
