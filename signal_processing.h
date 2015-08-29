// this file is distributed under 
// GPL v 3.0 license
#ifndef BYeAoSgG
#define BYeAoSgG
#include <random>
#include "photon2signal.h"
class Single2SingleSignal:public SignalAcceptor,public SignalProducent{
	virtual void AcceptEventStart()final;
	virtual void AcceptSignalValue(double signal)=0;
	virtual void AcceptEventEnd()final;
};
class Signal:public Single2SingleSignal{
public:
	Signal();
	virtual ~Signal();
	virtual void AcceptSignalValue(double signal)final;
};
class SignalPolinomialDistort:public Single2SingleSignal{
public:
	SignalPolinomialDistort(Vec&&coefs);
	virtual ~SignalPolinomialDistort();
	virtual void AcceptSignalValue(double signal)final;
private:
	Vec m_coefs;
};
//There's a problem with transfering vector<smth>&& parameter as {val1,val2,...} to make_Shared template function
inline std::shared_ptr<SignalPolinomialDistort> PolynomDistort(Vec&&coefs){
	return std::shared_ptr<SignalPolinomialDistort>(new SignalPolinomialDistort(static_cast<Vec&&>(coefs)));
}
inline std::shared_ptr<SignalPolinomialDistort> SignalAdd(double v){return PolynomDistort({v,1});}
inline std::shared_ptr<SignalPolinomialDistort> SignalMultiply(double c){return PolynomDistort({0,c});}
inline std::shared_ptr<SignalPolinomialDistort> SignalInvert(){return SignalMultiply(-1);}
class AmplitudeDiscriminator:public Single2SingleSignal{
public:
	AmplitudeDiscriminator(double thr);
	virtual ~AmplitudeDiscriminator();
    virtual void AcceptSignalValue(double signal)final;
private:
	double threshold;
};
class AbstractMultiInput:public std::enable_shared_from_this<AbstractMultiInput>{
	class Slot;
	friend class Slot;
public:
	AbstractMultiInput();
	virtual ~AbstractMultiInput();
	AbstractMultiInput&operator<<(std::shared_ptr<SignalProducent> input);
protected:
	virtual void Start()=0;
	virtual void Process(const Vec&signals)=0;
	virtual void Finish()=0;
	void OneChannelBegin();
	void OneChannelEnd();
private:
	class Slot:public SignalAcceptor{
	public:
		Slot(std::shared_ptr<AbstractMultiInput>father);
        virtual ~Slot();
		virtual void AcceptEventStart()final;
		virtual void AcceptSignalValue(double signal)final;
		virtual void AcceptEventEnd()final;
		double Value();
	private:
		std::shared_ptr<AbstractMultiInput>master;
		double m_value;
	};
	std::vector<std::shared_ptr<Slot>> m_input_slots;
	int m_state;
};
class Multi2SingleSignal:public AbstractMultiInput,public SignalProducent{
public:
    Multi2SingleSignal();
	virtual ~Multi2SingleSignal();
protected:
	virtual void Start()final;
	virtual void Finish()final;
};
class SignalSumm:public Multi2SingleSignal{
public:
	SignalSumm();
	virtual ~SignalSumm();
protected:
	virtual void Process(const Vec&signals)final;
};
class SignalProduct:public Multi2SingleSignal{
public:
	SignalProduct();
	virtual ~SignalProduct();
protected:
	virtual void Process(const Vec&signals)final;
};
class SignalSortAndSelect:public Multi2SingleSignal{
public:
	SignalSortAndSelect(size_t number);
	virtual ~SignalSortAndSelect();
protected:
	virtual void Process(const Vec&signals)final;
private:
	size_t m_number;
};
class AbstractMultiOutput{
public:
	AbstractMultiOutput();
	virtual ~AbstractMultiOutput();
	AbstractMultiOutput&operator>>(std::shared_ptr<SignalAcceptor>output);
protected:
	void SendEventStart();
	size_t GetOutSlotsCount();
	void SendSignalValue(size_t i,double signal);
	void SendEventEnd();
private:
	class Slot:public SignalProducent{
	public:
		Slot();
		virtual ~Slot();
		void Start();
		void Value(double signal);
		void End();
	};
	std::vector<std::shared_ptr<Slot>> m_output_slots;
};
class Single2MultiSignal:public SignalAcceptor,public AbstractMultiOutput{
public:
    Single2MultiSignal();
    virtual ~Single2MultiSignal();
	virtual void AcceptEventStart()final;
	virtual void AcceptSignalValue(double time)=0;
	virtual void AcceptEventEnd()final;
};
class Multi2MultiSignal:public AbstractMultiInput,public AbstractMultiOutput{
public:
    Multi2MultiSignal();
    virtual ~Multi2MultiSignal();
protected:
	virtual void Start()final;
	virtual void Process(const Vec&signals)=0;
	virtual void Finish()final;
};
class TimeGate:public Multi2MultiSignal{
public:
    TimeGate(double width);
    virtual ~TimeGate();
protected:
	virtual void Process(const Vec&signals)override;
private:
	double m_width;
};
class AllSignalsPresent:public Multi2MultiSignal{
public:
    AllSignalsPresent(){}
    virtual ~AllSignalsPresent(){}
protected:
	virtual void Process(const Vec&signals);
};

class SignalSortAndSelect2:public Multi2MultiSignal{
public:
	SignalSortAndSelect2(size_t number);
	virtual ~SignalSortAndSelect2();
protected:
	virtual void Process(const Vec&signals)final;
private:
	size_t m_number;
};
class SignalSort:public Multi2MultiSignal{
public:
	SignalSort(){}
	virtual ~SignalSort(){}
protected:
	virtual void Process(const Vec&signals)final;
};
class SignalSort2:public Multi2MultiSignal{
public:
	SignalSort2(){}
	virtual ~SignalSort2(){}
protected:
	virtual void Process(const Vec&signals)final;
};
#endif 