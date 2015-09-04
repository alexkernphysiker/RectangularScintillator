// this file is distributed under 
// GPL v 3.0 license
#ifndef bIRWVXRF
#define bIRWVXRF
#include <gtest/gtest.h>
#include <math_h/exception_math_h.h>
#include <geometry.h>
#include <rectscin.h>
#include <sensitive.h>
#include <lightguide.h>
#include <photon2signal.h>
#include <signal_processing.h>
#include <signal_statistics.h>
using namespace std;
extern RANDOM engine;
extern uniform_real_distribution<double> Rand;
extern uniform_int_distribution<int> iRand;
#define EXPECT_CLOSE_VALUES_with_error(A,B,e) EXPECT_TRUE(((A)>=((B)-(e)))&&((A)<=((B)+(e))));
#define EXPECT_CLOSE_VALUES(A,B) EXPECT_TRUE(((A)>=((B)-(0.000001)))&&((A)<=((B)+(0.000001))));
class SignalSender{
public:
	SignalSender();
	~SignalSender();
	SignalSender& operator>>(shared_ptr<SignalAcceptor>);
	SignalSender& Connect2MultiInput(shared_ptr<AbstractMultiInput>input,size_t count);
	SignalSender& send(Vec&&signals);
private:
	vector<shared_ptr<SignalAcceptor>> m_slots;
};
class Out:public SignalAcceptor{
public:
	Out();
	virtual ~Out();
	virtual void AcceptEventStart()final;
	virtual void AcceptSignalValue(double s)final;
	virtual void AcceptEventEnd()final;
	double value();
private:
	double signal;
	bool event;
};
#endif 