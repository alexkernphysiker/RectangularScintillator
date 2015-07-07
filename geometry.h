#ifndef rccNvDvy
#define rccNvDvy
#include <utility>
#include <vector>
typedef std::vector<double> Vec;
typedef std::pair<double,double> Pair;
Vec operator*(Vec&p,double c);
Vec operator+(Vec&p1,Vec&p2);
double SqDistance(Vec&p1,Vec&p2);
double Distance(Vec&p1,Vec&p2);
class RectDimensions{
public:
	struct IntersectionSearchResults{
		unsigned int SurfaceDimentionIndex;
		enum Side{None,Left,Right};
		Side Surface;
		double K;
		Vec Coordinates;
	};
	RectDimensions();
	virtual ~RectDimensions();
	RectDimensions &operator<<(Pair&&dimension);
	unsigned int NumberOfDimensions();
	Pair &&Dimension(unsigned int i);
	bool IsInside(Vec point);
	IntersectionSearchResults WhereIntersects(Vec&&point,Vec&&dir);
private:
	std::vector<Pair> m_dimensions;
};
#endif