// this file is distributed under 
// GPL v 3.0 license
#include "test_objects.h"
TEST(SignalStatictics,Base){
	auto test=make_shared<SignalStatictics>();
	SignalSender s;
	s>>test;
	EXPECT_EQ(0,test->data().count());
	for(size_t i=0;i<10;i++){
		for(size_t j=1;j<=i;j++){
			s.send({rand()});
			EXPECT_EQ(j,test->data().count());
		}
		test->Clear();
		EXPECT_EQ(0,test->data().count());
	}
}
TEST(SignalDistribution,Base){
	for(size_t a=0;a<7;a++){
		double f=(rand()%50)-25,t=f+(rand()%50)+1;
		int cnt=rand()%100+2;
		SignalDistribution test(f,t,cnt);
		EXPECT_CLOSE_VALUES(f+test.data().BinWidth()/2.0,test.data().min());
		EXPECT_CLOSE_VALUES(t-test.data().BinWidth()/2.0,test.data().max());
		EXPECT_EQ(cnt,test.data().size());
	}
}
TEST(Signal2DCorrelation,Base){
	auto test=make_shared<Signal2DCorrelation>();
	SignalSender sig;
	sig.Connect2MultiInput(test,2);
	EXPECT_EQ(0,test->Points().size());
	for(size_t i=0;i<10;i++){
		for(size_t j=1;j<=i;j++){
			double a=rand(),b=rand();
			sig.send({a,b});
			EXPECT_EQ(j,test->Points().size());
			EXPECT_EQ(a,test->Points()[j-1].first);
			EXPECT_EQ(b,test->Points()[j-1].second);
		}
		test->Clear();
		EXPECT_EQ(0,test->Points().size());
	}
}
TEST(Signal2DCorrelation,Throw){
	auto test=make_shared<Signal2DCorrelation>();
	SignalSender sig;
	sig.Connect2MultiInput(test,1);
	EXPECT_THROW(sig.send({rand()}),RectScinException);
	sig.Connect2MultiInput(test,1);
	EXPECT_NO_THROW(sig.send({rand(),rand()}));
	sig.Connect2MultiInput(test,1);
	EXPECT_THROW(sig.send({rand(),rand(),rand()}),RectScinException);
}