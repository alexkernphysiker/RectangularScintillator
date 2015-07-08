// this file is distributed under 
// GPL v 3.0 license
#include <exception>
#include <math.h>
#include "math_h/interpolate.h"
#include "math_h/randomfunc.h"
#include "geometry.h"
using namespace std;
Vec operator*(Vec&&p,double c){
	Vec res;
	for(double x:p)
		res.push_back(x*c);
	return res;
}
Vec operator+(Vec&&p1,Vec&&p2){
	if(p1.size()!=p2.size())
		throw exception();
	Vec res;
	for(unsigned int i=0,n=p1.size();i<n;i++)
		res.push_back(p1[i]+p2[i]);
	return res;
}
Vec operator-(Vec&&p1,Vec&&p2){
	if(p1.size()!=p2.size())
		throw exception();
	Vec res;
	for(unsigned int i=0,n=p1.size();i<n;i++)
		res.push_back(p1[i]-p2[i]);
	return res;
}
double SqAbs(Vec&&p){
	double res=0;
	for(double x:p)res+=x*x;
	return res;
}
double Abs(Vec&& p){
	return sqrt(SqAbs(static_cast<Vec&&>(p)));
}
double SqDistance(Vec&&p1, Vec&&p2){
	return SqAbs(static_cast<Vec&&>(p1)-static_cast<Vec&&>(p2));
}
double Distance(Vec&&p1,Vec&&p2){
	return Abs(static_cast<Vec&&>(p1)-static_cast<Vec&&>(p2));
}
RectDimensions::RectDimensions(){}
RectDimensions::~RectDimensions(){}
RectDimensions& RectDimensions::operator<<(Pair&&dimension){
	if(dimension.first>dimension.second)
		throw exception();
	m_dimensions.push_back(dimension);
	return *this;
}
unsigned int RectDimensions::NumberOfDimensions(){
	return m_dimensions.size();
}
Pair&&RectDimensions::Dimension(unsigned int i){
	if(i>=NumberOfDimensions())
		throw exception();
	return static_cast<Pair&&>(m_dimensions[i]);
}
bool RectDimensions::IsInside(Vec&&point){
	if(point.size()!=NumberOfDimensions())
		throw exception();
	for(unsigned int i=0,n=NumberOfDimensions();i<n;i++)
		if((point[i]<Dimension(i).first)||(point[i]>Dimension(i).second))
			return false;
	return true;
}
RectDimensions::IntersectionSearchResults RectDimensions::WhereIntersects(Vec&&point,Vec&&dir){
	if(!IsInside(static_cast<Vec&&>(point))){
		IntersectionSearchResults res;
		res.Surface=IntersectionSearchResults::None;
		return res;
	}
	typedef pair<double,unsigned int> dist_dim;
	vector<dist_dim> dim_order;
	for(unsigned int i=0,n=NumberOfDimensions();i<n;i++){
		if(dir[i]<0)
			InsertSorted(make_pair(point[i]-m_dimensions[i].first,i),dim_order,std_size(dim_order),std_insert(dim_order,dist_dim));
		if(dir[i]>0)
			InsertSorted(make_pair(m_dimensions[i].second-point[i],i),dim_order,std_size(dim_order),std_insert(dim_order,dist_dim));
	}
	if(dim_order.size()==0){
		IntersectionSearchResults res;
		res.Surface=IntersectionSearchResults::None;
		return res;
	}
	for(unsigned int i=0,n=dim_order.size();i<n;i++){
		unsigned int dimension=dim_order[i].second;
		double endx=dim_order[i].first;
		double k=(endx-point[dimension])/dir[dimension];
		if(k<0)throw exception();
		Vec endpoint=static_cast<Vec&&>(point)+(static_cast<Vec&&>(dir)*k);
		if(IsInside(static_cast<Vec&&>(endpoint))){
			IntersectionSearchResults res;
			if(dir[dimension]<0)
				res.Surface=IntersectionSearchResults::Left;
			else
				res.Surface=IntersectionSearchResults::Right;
			res.SurfaceDimentionIndex=dimension;
			res.K=k;
			for(double x:endpoint)res.Coordinates.push_back(x);
			return res;
		}
	}
	throw exception();
}