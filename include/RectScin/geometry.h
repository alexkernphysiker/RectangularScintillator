// this file is distributed under 
// MIT license
#ifndef rccNvDvy
#define rccNvDvy
#include <utility>
#include <vector>
#include <thread>
#include <mutex>
namespace RectangularScintillator{
	using namespace std;
	typedef lock_guard<mutex> Lock;
	typedef vector<double> Vec;
	typedef pair<double,double> Pair;
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
		vector<Pair> m_dimensions;
		mutex geom_mutex;
	};
	RectDimensions::Side&inc(RectDimensions::Side&val);
};
#endif