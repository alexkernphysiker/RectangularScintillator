// this file is distributed under 
// GPL v 3.0 license
#include <exception>
#include <random>
#include <gtest/gtest.h>
#include <geometry.h>
using namespace std;
default_random_engine rnd;
uniform_real_distribution<double> uniform(-10,10);
uniform_int_distribution<int> iuniform(-10,10);
TEST(VecOpr,throwtest){
	Vec A;
	Vec B;
	for(int i=0;i<10;i++){
		A.push_back(uniform(rnd));
		EXPECT_THROW(static_cast<Vec&&>(A)+static_cast<Vec&&>(B),exception);
		EXPECT_THROW(static_cast<Vec&&>(A)-static_cast<Vec&&>(B),exception);
		B.push_back(uniform(rnd));
		EXPECT_NO_THROW(static_cast<Vec&&>(A)+static_cast<Vec&&>(B));
		EXPECT_NO_THROW(static_cast<Vec&&>(A)-static_cast<Vec&&>(B));
	}
	A.clear();B.clear();
	for(int i=0;i<10;i++){
		B.push_back(uniform(rnd));
		EXPECT_THROW(static_cast<Vec&&>(A)+static_cast<Vec&&>(B),exception);
		EXPECT_THROW(static_cast<Vec&&>(A)-static_cast<Vec&&>(B),exception);
		A.push_back(uniform(rnd));
		EXPECT_NO_THROW(static_cast<Vec&&>(A)+static_cast<Vec&&>(B));
		EXPECT_NO_THROW(static_cast<Vec&&>(A)-static_cast<Vec&&>(B));
	}
}
TEST(VecOpr,Mul){
	Vec A;
	for(int i=0;i<10;i++){
		for(double c=-10;c<=10;c+=1)
			EXPECT_TRUE((static_cast<Vec&&>(A)*c).size()==A.size());
		A.push_back(uniform(rnd));
	}
	A.clear();
	for(int i=0;i<10;i++){
		for(double c=-9.5;c<10;c+=1){//escape zero
			Vec B=static_cast<Vec&&>(A)*c;
			Vec C=static_cast<Vec&&>(B)*(1.0/c);
			for(unsigned int i=0,n=A.size();i<n;i++){
				EXPECT_TRUE(C[i]==A[i]);
				EXPECT_TRUE(B[i]==(A[i]*c));
			}
		}
		A.push_back(iuniform(rnd));
	}
}
TEST(VecOpr,Add_Sub){
	Vec A;
	Vec B;
	for(int i=0;i<10;i++){
		for(double c=-10;c<=10;c+=1){
			EXPECT_TRUE((static_cast<Vec&&>(A)+static_cast<Vec&&>(B)).size()==A.size());
			EXPECT_TRUE((static_cast<Vec&&>(A)-static_cast<Vec&&>(B)).size()==A.size());
		}
		A.push_back(uniform(rnd));
		B.push_back(uniform(rnd));
	}
	A.clear();
	B.clear();
	for(int i=0;i<10;i++){
		for(double c=-10;c<=10;c+=1){
			Vec C=static_cast<Vec&&>(A)+static_cast<Vec&&>(B);
			Vec D=static_cast<Vec&&>(C)-static_cast<Vec&&>(B);
			for(unsigned int i=0,n=A.size();i<n;i++){
				EXPECT_TRUE(D[i]==A[i]);
				EXPECT_TRUE(C[i]==(A[i]+B[i]));
			}
		}
		A.push_back(uniform(rnd));
		B.push_back(uniform(rnd));
	}
}
