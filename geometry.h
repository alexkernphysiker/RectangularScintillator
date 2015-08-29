// this file is distributed under 
// GPL v 3.0 license
#ifndef rccNvDvy
#define rccNvDvy
#include <utility>
#include <vector>
#include <thread>
#include <mutex>
typedef std::lock_guard<std::mutex> Lock;
typedef std::vector<double> Vec;
typedef std::pair<double,double> Pair;
Vec operator*(const Vec&p,double c);
Vec operator+(const Vec&p1,const Vec&p2);
Vec operator-(const Vec&p1,const Vec&p2);
double SqAbs(const Vec&p);
double Abs(const Vec&p);
double SqDistance(const Vec&p1,const Vec&p2);
double Distance(const Vec&p1,const Vec&p2);
class RectDimensions{
public:
	enum Side{None=-1,Left=0,Right=1};
	struct IntersectionSearchResults{
		size_t surfaceDimentionIndex;
		Side surface;
		double k;
		Vec coordinates;
	};
	RectDimensions();
	virtual ~RectDimensions();
	RectDimensions &operator<<(Pair&&dimension);
	size_t NumberOfDimensions()const;
	Pair&&Dimension(size_t i)const;
	bool IsInside(const Vec&point)const;
	IntersectionSearchResults WhereIntersects(const Vec&point,const Vec&dir);
private:
	std::vector<Pair> m_dimensions;
	std::mutex geom_mutex;
};
RectDimensions::Side&inc(RectDimensions::Side&val);
#endif