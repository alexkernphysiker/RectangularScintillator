// this file is distributed under 
// GPL v 3.0 license
#ifndef mxhvHUgn
#define mxhvHUgn
#include "math_h/sigma.h"
#include "photon2signal.h"
class SignalStatictics:public SignalAcceptor,public Sigma<double>{
public:
    SignalStatictics();
    virtual ~SignalStatictics();
	virtual void AcceptEventStart()override;
	virtual void AcceptSignalValue(double time)override;
	virtual void AcceptEventEnd()override;
};
#endif 