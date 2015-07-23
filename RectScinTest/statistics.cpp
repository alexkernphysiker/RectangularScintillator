// this file is distributed under 
// GPL v 3.0 license
#include <gtest/gtest.h>
#include <rectscinexception.h>
#include <signal_statistics.h>
using namespace std;
#define EXPECT_CLOSE_VALUES(A,B) EXPECT_TRUE(pow((A)-(B),2)<0.0000001);
TEST(SignalStatictics,Base){
	SignalStatictics test;
	EXPECT_EQ(0,test.data().count());
	for(size_t i=0;i<10;i++){
		for(size_t j=1;j<=i;j++){
			test.AcceptSignalValue(rand());
			EXPECT_EQ(j,test.data().count());
		}
		test.Clear();
		EXPECT_EQ(0,test.data().count());
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
