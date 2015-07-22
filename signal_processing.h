// this file is distributed under 
// GPL v 3.0 license
#ifndef BYeAoSgG
#define BYeAoSgG
#include "photon2signal.h"
class SignalPolinomialDistort:public SignalAcceptor,public SignalProducent{
public:
	SignalPolinomialDistort(Vec&&coefs);
	virtual ~SignalPolinomialDistort();
	virtual void AcceptEventStart()override;
	virtual void AcceptSignalValue(double time)override;
	virtual void AcceptEventEnd()override;
private:
	Vec m_coefs;
};
class Multi2SingleSignal:public SignalProducent{
/*public:
	Multi2SingleSignal();
    virtual ~Multi2SingleSignal();*/
};
class Single2MultiSignal{};
class Multi2MultiSignal{};
#endif 