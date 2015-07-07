#ifndef WEXXxads
#define WEXXxads
#include <functional>
#include <memory>
#include "math_h/randomfunc.h"
#include "geometry.h"
const double speed_of_light=300;//mm/ns
typedef std::function<double(double)> Func;
struct Photon{
	Vec coord;
	double time;
	Vec dir;
	double lambda;
};
class IPhotoSensitive{
public:
	virtual ~IPhotoSensitive(){}
	virtual void RegisterPhoton(Photon&photon)=0;
};
class RectangularScintillator;
class PhotoSensitiveSurface:protected virtual RectDimensions{
	friend class RectangularScintillator;
public:
	PhotoSensitiveSurface();
	virtual ~PhotoSensitiveSurface();
	PhotoSensitiveSurface&operator<<(std::shared_ptr<IPhotoSensitive>handler);
protected:
	void RegisterPhoton(Photon&photon);//changes photon
private:
	std::vector<std::shared_ptr<IPhotoSensitive>> m_handlers;
};
class RectangularScintillator:protected virtual RectDimensions{
public:
	RectangularScintillator(std::vector<Pair> dimensions,
					Func time_distribution,double tmax,unsigned int tbins,
					Func lambda_distribution,double lmin,double lmax,unsigned int lbins,
					Func refraction,Func absorption
	);
	virtual ~RectangularScintillator();
	PhotoSensitiveSurface&Surface(unsigned int dimension,IntersectionSearchResults::Side side);
	void RegisterGamma(Vec&&coord,unsigned int N);
protected:
	Photon GeneratePhoton(Vec&&coord);
	IntersectionSearchResults TraceGeometry(Photon &ph);//Changes Photon
private:
	RandomValueGenerator<double> m_time_distribution;
	RandomValueGenerator<double> m_lambda_distribution;
	Func m_refraction;//depends on lambda
	Func m_absorption;//depends on lambda
	typedef std::pair<PhotoSensitiveSurface,PhotoSensitiveSurface> SurfPair;
	std::vector<SurfPair> m_edges;
	std::default_random_engine rand;
};

#endif