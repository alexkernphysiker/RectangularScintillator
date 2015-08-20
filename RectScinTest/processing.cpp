// this file is distributed under 
// GPL v 3.0 license
#include <math_h/functions.h>
#include "test_objects.h"
TEST(SignalPolinomialDistort,Base){
	for(size_t p=0;p<10;p++){
		Vec C;for(size_t i=0;i<=p;i++)C.push_back(Rand(engine));
		auto test=PolynomDistort(static_right(C));
		SignalSender sender;auto out=make_shared<Out>();
		sender>>(test>>out);
		double signal=Rand(engine);
		sender.send({signal});
		double expected=Polynom(signal,C,p);
		EXPECT_CLOSE_VALUES(out->value(),expected);
	}
}
TEST(SignalSumm,Base){
	for(size_t n=1;n<=10;n++){
		Vec signals;
		double expected=0;
		for(size_t i=0;i<n;i++){
			double s=Rand(engine);
			signals.push_back(s);
			expected+=s;
		}
		auto test=make_shared<SignalSumm>();
		SignalSender sender;auto out=make_shared<Out>();
		test>>out;
		sender.Connect2MultiInput(test,n);
		sender.send(static_right(signals));
		EXPECT_CLOSE_VALUES(out->value(),expected);
	}
}
TEST(SignalProduct,Base){
	for(size_t n=1;n<=10;n++){
		Vec signals;
		double expected=1;
		for(size_t i=0;i<n;i++){
			double s=Rand(engine);
			signals.push_back(s);
			expected*=s;
		}
		auto test=make_shared<SignalProduct>();
		SignalSender sender;auto out=make_shared<Out>();
		test>>out;
		sender.Connect2MultiInput(test,n);
		sender.send(static_right(signals));
		EXPECT_CLOSE_VALUES(out->value(),expected);
	}
}
TEST(AmplitudeDiscriminator,SimpleTest){
	for(double thr=-10;thr<=10;thr+=1){
		auto test=make_shared<AmplitudeDiscriminator>(thr);
		SignalSender sender;sender>>test;
		auto out=make_shared<Out>();test>>out;
		for(double signal=-11;signal>=11;signal+=1){
			sender.send({signal});
			if(signal<thr)
				EXPECT_FALSE(isfinite(out->value()));
			else
				EXPECT_EQ(signal,out->value());
		}
	}
}
TEST(SignalSortAndSelect,BaseTest){
	for(size_t n=1;n<=10;n++)
		for(size_t orderstatistics=0;orderstatistics<n+5;orderstatistics++){
			auto test=make_shared<SignalSortAndSelect>(orderstatistics);
			SignalSender sender;auto out=make_shared<Out>();
			test>>out; sender.Connect2MultiInput(test,n);
			uniform_int_distribution<int> indexr(0,100000);
			for(size_t k=0;k<50;k++){
				Vec signals;
				for(double v=0;v<n;v++)
					if(signals.size()==0)
						signals.push_back(v);
					else
						signals.insert(signals.begin()+indexr(engine)%(signals.size()+1),v);
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
TEST(SignalSortAndSelect2,BaseTest){
	for(size_t n=1;n<=50;n++)
		for(size_t orderstatistics=0;orderstatistics<n+5;orderstatistics++){
			auto test=make_shared<SignalSortAndSelect2>(orderstatistics);
			SignalSender sender;auto out1=make_shared<Out>(),out2=make_shared<Out>();
			test>>out1>>out2; sender.Connect2MultiInput(test,n);
			for(size_t k=0;k<100;k++){
				Vec signals;
				uniform_int_distribution<int> indexr(0,100000);
				for(double v=0;v<n;v++)
					if(signals.size()==0)
						signals.push_back(v);
					else
						signals.insert(signals.begin()+indexr(engine)%(signals.size()+1),v);
					if(orderstatistics<n)
						EXPECT_NO_THROW(sender.send(static_right(signals)));
					else
						EXPECT_THROW(sender.send(static_right(signals)),RectScinException);
					if(orderstatistics<n){
						EXPECT_TRUE(out1->value()>=0);
						EXPECT_TRUE(out1->value()<signals.size());
						EXPECT_EQ(orderstatistics,out2->value());
						for(size_t i=0,n=signals.size();i<n;i++)
							if(orderstatistics==signals[i])
								EXPECT_EQ(i,out1->value());
							else
								EXPECT_NE(i,out1->value());
					}else{
						EXPECT_FALSE(isfinite(out1->value()));
						EXPECT_FALSE(isfinite(out2->value()));
					}
			}
		}
}
TEST(TimeGate,SimpleTest){
	EXPECT_THROW(make_shared<TimeGate>(-1),RectScinException);
	EXPECT_THROW(make_shared<TimeGate>(0),RectScinException);
	for(double d=1;d<10;d+=1){
		auto test=make_shared<TimeGate>(d);
		auto out1=make_shared<Out>(),out2=make_shared<Out>();
		SignalSender sender;
		sender.Connect2MultiInput(test,2);
		test>>out1>>out2;
		for(double x1=-10;x1<=10;x1+=1)
			for(double x2=-10;x2<=10;x2+=1){
				sender.send({x1,x2});
				if(x2<x1)
					EXPECT_FALSE(isfinite(out1->value())||isfinite(out2->value()));
				else{
					if(x2>(x1+d))
						EXPECT_FALSE(isfinite(out1->value())||isfinite(out2->value()));
					else{
						EXPECT_EQ(x1,out1->value());
						EXPECT_EQ(x2,out2->value());
					}
				}
			}
	}
	
}
TEST(AllSignalsPresent,SimpleTest){
	auto test=make_shared<AllSignalsPresent>();
	auto out1=make_shared<Out>(),out2=make_shared<Out>();
	SignalSender sender;
	sender.Connect2MultiInput(test,2);
	test>>out1>>out2;
	for(double x1=-10;x1<=10;x1+=1)
		for(double x2=-10;x2<=10;x2+=1){
			sender.send({x1,x2});
			EXPECT_TRUE(isfinite(out1->value()));
			EXPECT_TRUE(isfinite(out2->value()));
			EXPECT_EQ(x1,out1->value());
			EXPECT_EQ(x2,out2->value());
		}
	for(double x1=-10;x1<=10;x1+=1){
		sender.send({x1,INFINITY});
		EXPECT_FALSE(isfinite(out1->value()));
		EXPECT_FALSE(isfinite(out2->value()));
	}
	for(double x2=-10;x2<=10;x2+=1){
		sender.send({INFINITY,x2});
		EXPECT_FALSE(isfinite(out1->value()));
		EXPECT_FALSE(isfinite(out2->value()));
	}
}
