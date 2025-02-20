// this file is distributed under 
// MIT license
#include <math_h/functions.h>
#include "test_objects.h"
using namespace std;
using namespace MathTemplates;
using namespace RectangularScintillator;
TEST(AmplitudeSignal, BaseTest) {
	auto test = make_shared<AmplitudeSignal>();
	auto out = make_shared<Out>();
	test >> out;
	for (size_t n = 0;n < 100;n++) {
		test->AcceptEventStart();
		for (size_t i = 0;i < n;i++)
			test->AcceptPhotonTime(double(i) / 100);
		test->AcceptEventEnd();
		EXPECT_EQ(n, out->value());
	}
}
TEST(WeightedTimeSignal, IfNotComplete) {
	for (size_t n = 1;n < 100;n++) {
		vector<pair<size_t, double>> vec;
		for (size_t i = 0;i < n;i++)
			vec.push_back(make_pair(i, 1));
		auto test = TimeSignal(static_cast<decltype(vec)>(vec));
		auto out = make_shared<Out>();
		test >> out;
		for (size_t cnt = 0;cnt < n;cnt++) {
			test->AcceptEventStart();
			for (size_t i = 0;i < cnt;i++)
				test->AcceptPhotonTime(i);
			test->AcceptEventEnd();
			EXPECT_FALSE(isfinite(out->value()));
		}
	}
}
TEST(WeightedTimeSignal, IfComplete) {
	for (size_t n = 1;n < 100;n++) {
		vector<pair<size_t, double>> vec;
		for (size_t i = 0;i < n;i++)
			vec.push_back(make_pair(i, 1));
		auto test = TimeSignal(static_cast<decltype(vec)>(vec));
		auto out = make_shared<Out>();
		test >> out;
		test->AcceptEventStart();
		int expect = 0;
		for (size_t i = 0;i < n;i++) {
			test->AcceptPhotonTime(i);
			expect += i;
		}
		test->AcceptEventEnd();
		EXPECT_EQ(expect, out->value());
	}
}
