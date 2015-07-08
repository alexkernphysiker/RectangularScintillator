#ifndef CilPeTQJ
#define CilPeTQJ
#include "math_h/sigma.h"
#include "sensitive.h"
class Counter:public ISignal{
public:
	Counter();
    virtual ~Counter();
	virtual void Start()override;
	virtual void Photon(double time)override;
	virtual void End()override;
	unsigned int count();
	double average();
	double sigma();
private:
	unsigned int current;
	Sigma<double> m_count;
};
#endif