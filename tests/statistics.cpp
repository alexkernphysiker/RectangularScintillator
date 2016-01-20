// this file is distributed under 
// MIT license
#include "test_objects.h"
using namespace std;
using namespace MathTemplates;
using namespace RectangularScintillator;
TEST(SignalStatictics,Base){
	auto test=make_shared<SignalStatictics>();
	SignalSender s;
	s>>test;
	EXPECT_EQ(0,test->data().count());
	for(size_t i=0;i<10;i++){
		for(size_t j=1;j<=i;j++){
			s.send({Rand(engine)});
			EXPECT_EQ(j,test->data().count());
		}
		test->Clear();
		EXPECT_EQ(0,test->data().count());
	}
}
TEST(SignalDistribution,Base){
	for(size_t a=0;a<7;a++){
		double f=Rand(engine),t=f+Rand(engine)+10;
		int cnt=rand()%100+2;
		SignalDistribution test(f,t,cnt);
		EXPECT_CLOSE_VALUES(f+test.data().BinWidth()/2.0,test.data().min());
		EXPECT_CLOSE_VALUES(t-test.data().BinWidth()/2.0,test.data().max());
		EXPECT_EQ(cnt,test.data().size());
	}
}
