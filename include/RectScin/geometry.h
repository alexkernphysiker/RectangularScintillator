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
	Vec operator*(const Vec&p,const double c);
	Vec operator+(const Vec&p1,const Vec&p2);
	Vec operator-(const Vec&p1,const Vec&p2);
	inline Vec operator*(const Vec&&p,const double c){return p*c;}
	inline Vec operator+(const Vec&&p1,const Vec&p2){return p1+p2;}
	inline Vec operator-(const Vec&&p1,const Vec&p2){return p1-p2;}
	inline Vec operator+(const Vec&p1,const Vec&&p2){return p1+p2;}
	inline Vec operator-(const Vec&p1,const Vec&&p2){return p1-p2;}
	inline Vec operator+(const Vec&&p1,const Vec&&p2){return p1+p2;}
	inline Vec operator-(const Vec&&p1,const Vec&&p2){return p1-p2;}
	double SqAbs(const Vec&p);
	double Abs(const Vec&p);
	inline double SqAbs(const Vec&&p){return SqAbs(p);}
	inline double Abs(const Vec&&p){return Abs(p);}
	double SqDistance(const Vec&p1,const Vec&p2);
	inline double SqDistance(const Vec&&p1,const Vec&p2){return SqDistance(p1,p2);}
	inline double SqDistance(const Vec&p1,const Vec&&p2){return SqDistance(p1,p2);}
	inline double SqDistance(const Vec&&p1,const Vec&&p2){return SqDistance(p1,p2);}
	double Distance(const Vec&p1,const Vec&p2);
	inline double Distance(const Vec&&p1,const Vec&p2){return Distance(p1,p2);}
	inline double Distance(const Vec&p1,const Vec&&p2){return Distance(p1,p2);}
	inline double Distance(const Vec&&p1,const Vec&&p2){return Distance(p1,p2);}
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
		RectDimensions&operator<<(const Pair&dimension);
		RectDimensions&operator<<(const Pair&&dimension);
		const size_t NumberOfDimensions()const;
		const Pair&Dimension(size_t i)const;
		const bool IsInside(const Vec&point)const;
		const IntersectionSearchResults WhereIntersects(const Vec&point,const Vec&dir)const;
	private:
		vector<Pair> m_dimensions;
		shared_ptr<mutex> geom_mutex;
	};
	RectDimensions::Side&inc(RectDimensions::Side&val);
};
#endif