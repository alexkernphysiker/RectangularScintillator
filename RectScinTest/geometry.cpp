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
		EXPECT_THROW(SqDistance(static_cast<Vec&&>(A),static_cast<Vec&&>(B)),exception);
		EXPECT_THROW(Distance(static_cast<Vec&&>(A),static_cast<Vec&&>(B)),exception);
		B.push_back(uniform(rnd));
		EXPECT_NO_THROW(static_cast<Vec&&>(A)+static_cast<Vec&&>(B));
		EXPECT_NO_THROW(static_cast<Vec&&>(A)-static_cast<Vec&&>(B));
		EXPECT_NO_THROW(SqDistance(static_cast<Vec&&>(A),static_cast<Vec&&>(B)));
		EXPECT_NO_THROW(Distance(static_cast<Vec&&>(A),static_cast<Vec&&>(B)));
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
TEST(VecOpr,Distances){
	Vec A;
	Vec B;
	for(int i=0;i<10;i++){
		for(double c=-10;c<=10;c+=1){
			double d=Distance(static_cast<Vec&&>(A),static_cast<Vec&&>(B));
			EXPECT_TRUE(d>=0);
			EXPECT_TRUE(sqrt(SqDistance(static_cast<Vec&&>(A),static_cast<Vec&&>(B)))==d);
		}
		A.push_back(uniform(rnd));
		B.push_back(uniform(rnd));
	}
	A.clear();
	B.clear();
	A.push_back(uniform(rnd));
	B.push_back(uniform(rnd));
	EXPECT_TRUE(SqDistance(static_cast<Vec&&>(A),static_cast<Vec&&>(B))==pow(A[0]-B[0],2));
}

TEST(RectDimensions,Dimensions){
	RectDimensions A;
	EXPECT_TRUE(A.NumberOfDimensions()==0);
	EXPECT_THROW(A.Dimension(0),exception);
	EXPECT_THROW(A.Dimension(1),exception);
	EXPECT_THROW(A.Dimension(2),exception);
	for(unsigned int i=1;i<10;i++){
		EXPECT_TRUE(&A==&(A<<make_pair(i,2*i)));
		EXPECT_TRUE(A.NumberOfDimensions()==i);
		for(unsigned int j=1;j<=i;j++){
			EXPECT_TRUE(A.Dimension(j-1).first==j);
			EXPECT_TRUE(A.Dimension(j-1).second==2*j);
		}
		EXPECT_THROW(A.Dimension(i),exception);
		EXPECT_THROW(A.Dimension(i+1),exception);
		EXPECT_THROW(A.Dimension(i+2),exception);
	}
}
TEST(RectDimensions,IsInside){
	RectDimensions A;
	for(unsigned int i=1;i<10;i++){
		A<<make_pair(-10.0+i,double(i));
		for(unsigned int cnt=0;cnt<(500*i);cnt++){
			bool test=true;
			Vec v;
			for(unsigned int j=0,n=A.NumberOfDimensions();j<n;j++){
				double x=uniform(rnd);
				test&=(x>=A.Dimension(j).first)&&(x<=A.Dimension(j).second);
				EXPECT_THROW(A.IsInside(static_cast<Vec&&>(v)),exception);
				v.push_back(x);
			}
			EXPECT_TRUE(A.IsInside(static_cast<Vec&&>(v))==test);
		}
	}
}
TEST(RectDimensions,WhereIntersects_throwing1){
	RectDimensions A;
	Vec O;Vec D;
	for(unsigned int i=1;i<10;i++){
		A<<make_pair(-10.0+i,double(i));
		EXPECT_THROW(A.WhereIntersects(static_cast<Vec&&>(O),static_cast<Vec&&>(D)),exception);
		O.push_back(0);
		EXPECT_THROW(A.WhereIntersects(static_cast<Vec&&>(O),static_cast<Vec&&>(D)),exception);
		D.push_back(0);
		EXPECT_TRUE(A.WhereIntersects(static_cast<Vec&&>(O),static_cast<Vec&&>(D)).Surface==RectDimensions::IntersectionSearchResults::None);
	}
}
TEST(RectDimensions,WhereIntersects_throwing2){
	RectDimensions A;
	Vec O;Vec D;
	for(unsigned int i=1;i<10;i++){
		A<<make_pair(-10.0+i,double(i));
		EXPECT_THROW(A.WhereIntersects(static_cast<Vec&&>(O),static_cast<Vec&&>(D)),exception);
		D.push_back(0);
		EXPECT_THROW(A.WhereIntersects(static_cast<Vec&&>(O),static_cast<Vec&&>(D)),exception);
		O.push_back(0);
		EXPECT_TRUE(A.WhereIntersects(static_cast<Vec&&>(O),static_cast<Vec&&>(D)).Surface==RectDimensions::IntersectionSearchResults::None);
	}
}
TEST(RectDimensions,WhereIntersects){
	for(unsigned int cnt=0;cnt<10000;cnt++){
		RectDimensions A;
		Vec O, Outside,D;
		for(unsigned int i=1;i<10;i++){
			A<<make_pair(-10.0+i,double(i));
			uniform_real_distribution<double> coord(-10.0+i,i);
			O.push_back(coord(rnd));
			Outside.push_back(100);
			D.push_back(uniform(rnd));
			EXPECT_TRUE(A.WhereIntersects(static_cast<Vec&&>(Outside),static_cast<Vec&&>(D)).Surface==RectDimensions::IntersectionSearchResults::None);
			if(Abs(static_cast<Vec&&>(D))>0){
				RectDimensions::IntersectionSearchResults res=A.WhereIntersects(static_cast<Vec&&>(O),static_cast<Vec&&>(D));
				if(res.Surface!=RectDimensions::IntersectionSearchResults::None){
					EXPECT_TRUE(res.SurfaceDimentionIndex<A.NumberOfDimensions());
					EXPECT_TRUE(res.K>=0);
					double diff;
					if(res.Surface==RectDimensions::IntersectionSearchResults::Left)
						diff=pow(res.Coordinates[res.SurfaceDimentionIndex]-A.Dimension(res.SurfaceDimentionIndex).first,2);
					else
						diff=pow(res.Coordinates[res.SurfaceDimentionIndex]-A.Dimension(res.SurfaceDimentionIndex).second,2);
					EXPECT_TRUE(diff<0.00000001);
					EXPECT_FALSE(diff<0);
				}
			}else{
				EXPECT_TRUE(A.WhereIntersects(static_cast<Vec&&>(O),static_cast<Vec&&>(D)).Surface==RectDimensions::IntersectionSearchResults::None);
			}
		}
	}
}
