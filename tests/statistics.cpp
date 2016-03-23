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
		s.send({Rand(engine)});
		for(size_t j=1;j<=i;j++){
			s.send({Rand(engine)});
			EXPECT_TRUE(test->data().val()>=Rand.min());
			EXPECT_TRUE(test->data().val()<=Rand.max());
		}
	}
}
