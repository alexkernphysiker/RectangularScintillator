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
#define static_right(A) (static_cast<decltype(A)&&>(A))
Vec operator*(Vec&&p,double c);
Vec operator+(Vec&&p1,Vec&&p2);
Vec operator-(Vec&&p1,Vec&&p2);
double SqAbs(Vec&&p);
double Abs(Vec&&p);
double SqDistance(Vec&&p1,Vec&&p2);
double Distance(Vec&&p1,Vec&&p2);
class RectDimensions{
public:
	enum Side{None=-1,Left=0,Right=1};
	struct IntersectionSearchResults{
		unsigned int surfaceDimentionIndex;
		Side surface;
		double k;
		Vec coordinates;
	};
	RectDimensions();
	virtual ~RectDimensions();
	RectDimensions &operator<<(Pair&&dimension);
	unsigned int NumberOfDimensions();
	Pair &&Dimension(unsigned int i);
	bool IsInside(Vec&&point);
	IntersectionSearchResults WhereIntersects(Vec&&point,Vec&&dir);
private:
	std::vector<Pair> m_dimensions;
	std::mutex geom_mutex;
};
RectDimensions::Side&operator++(RectDimensions::Side&val);
#endif