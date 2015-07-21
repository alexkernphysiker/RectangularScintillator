// this file is distributed under 
// GPL v 3.0 license
#include <mutex>
#include <math.h>
#include "math_h/interpolate.h"
#include "math_h/randomfunc.h"
#include "rectscinexception.h"
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
		throw RectScinException("A+B: vector sizes differ");
	Vec res;
	for(unsigned int i=0,n=p1.size();i<n;i++)
		res.push_back(p1[i]+p2[i]);
	return res;
}
Vec operator-(Vec&&p1,Vec&&p2){
	if(p1.size()!=p2.size())
		throw RectScinException("A-B: vector sizes differ");
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
	return sqrt(SqAbs(static_right(p)));
}
double SqDistance(Vec&&p1, Vec&&p2){
	return SqAbs(static_right(p1)-static_right(p2));
}
double Distance(Vec&&p1,Vec&&p2){
	return Abs(static_right(p1)-static_right(p2));
}
RectDimensions::Side& inc(RectDimensions::Side& val){
	val=static_cast<RectDimensions::Side>(val+1);
	return val;
}
RectDimensions::RectDimensions(){}
RectDimensions::~RectDimensions(){}
RectDimensions& RectDimensions::operator<<(Pair&&dimension){
	if(dimension.first>dimension.second)
		throw RectScinException("RectDimensions: wrong dimension left>right");
	Lock lock(geom_mutex);
	m_dimensions.push_back(dimension);
	return *this;
}
unsigned int RectDimensions::NumberOfDimensions(){
	Lock lock(geom_mutex);
	return m_dimensions.size();
}
Pair&&RectDimensions::Dimension(unsigned int i){
	if(i>=NumberOfDimensions())
		throw RectScinException("RectDimensions: dimension index out of range");
	Lock lock(geom_mutex);
	return static_cast<Pair&&>(m_dimensions[i]);
}
bool RectDimensions::IsInside(Vec&&point){
	if(point.size()!=NumberOfDimensions())
		throw RectScinException("RectDimensions::IsInside: wrong point size");
	for(unsigned int i=0,n=NumberOfDimensions();i<n;i++)
		if((point[i]<Dimension(i).first)||(point[i]>Dimension(i).second))
			return false;
	return true;
}
RectDimensions::IntersectionSearchResults RectDimensions::WhereIntersects(Vec&&point,Vec&&dir){
	if(NumberOfDimensions()!=dir.size())
		throw RectScinException("RectDimensions trace: wrong direction vector size");
	if(!IsInside(static_right(point))){
		IntersectionSearchResults res;
		res.surface=None;
		return res;
	}
	// Distance to the corresponding edge, dimension index
	typedef pair<double,unsigned int> dist_dim;
	vector<dist_dim> dim_order;
	for(unsigned int i=0,n=NumberOfDimensions();i<n;i++){
		dist_dim newdim;
		Lock lock(geom_mutex);
		newdim.second=i;
		if(dir[i]<0)
			newdim.first=point[i]-m_dimensions[i].first;
		if(dir[i]>0)
			newdim.first=m_dimensions[i].second-point[i];
		if(dir[i]!=0)
			InsertSorted(newdim,dim_order,std_size(dim_order),std_insert(dim_order,dist_dim));
	}
	if(dim_order.size()==0){
		IntersectionSearchResults res;
		res.surface=None;
		return res;
	}
	for(unsigned int i=0,n=dim_order.size();i<n;i++){
		unsigned int dimension=dim_order[i].second;
		double k=dim_order[i].first/dir[dimension];
		if(k<0) k=-k;
		Vec endpoint=static_right(point)+(static_right(dir)*k);
		bool Belong_to_surface=true;
		for(unsigned int i=0,n=NumberOfDimensions();i<n;i++)
			if(i!=dimension){
				Belong_to_surface &= endpoint[i]>=Dimension(i).first;
				Belong_to_surface &= endpoint[i]<=Dimension(i).second;
			}
		if(Belong_to_surface){
			IntersectionSearchResults res;
			if(dir[dimension]<0)
				res.surface=Left;
			else
				res.surface=Right;
			res.surfaceDimentionIndex=dimension;
			res.k=k;
			res.coordinates=endpoint;
			return res;
		}
	}
	IntersectionSearchResults res;
	res.surface=None;
	return res;
}