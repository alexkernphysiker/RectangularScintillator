// this file is distributed under 
// GPL v 3.0 license
#include <exception>
#include <gtest/gtest.h>
#include <geometry.h>
using namespace std;
TEST(VecOpr,throw){
	Vec A;
	Vec B;
	A.push_back(0);
	EXPECT_THROW(static_cast<Vec&&>(A)+static_cast<Vec&&>(B),exception);
}
