// this file is distributed under 
// GPL v 3.0 license
#ifndef mxhvHUgn
#define mxhvHUgn
#include "math_h/sigma.h"
#include "photon2signal.h"
class TimeSignalStatictics:public TimeSignalAcceptor,public Sigma<double>{
public:
    TimeSignalStatictics();
    virtual ~TimeSignalStatictics();
	virtual void AcceptEventStart()override;
	virtual void AcceptSignalTime(double time)override;
	virtual void AcceptEventEnd()override;
};
#endif 