// this file is distributed under 
// MIT license
#include "test_objects.h"
using namespace std;
using namespace MathTemplates;
using namespace RectangularScintillator;
TEST(SignalStatictics,Base){
	for(size_t i=0;i<10;i++){
		auto test=make_shared<SignalStatictics>();
		SignalSender s;
		s>>test;
		EXPECT_EQ(0,test->data().count());
		for(size_t j=1;j<=i;j++){
			s.send({Rand(engine)});
			EXPECT_EQ(j,test->data().count());
		}
	}
}
