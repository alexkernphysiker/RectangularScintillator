// this file is distributed under 
// GPL v 3.0 license
#ifndef BYeAoSgG
#define BYeAoSgG
#include <random>
#include "photon2signal.h"
class Single2SingleSignal:public SignalAcceptor,public SignalProducent{};
class Signal:public Single2SingleSignal{
public:
	Signal(){}
    virtual ~Signal(){}
	virtual void AcceptEventStart()final{SendEventStart();}
	virtual void AcceptSignalValue(double signal)final{SendSignalValue(signal);}
	virtual void AcceptEventEnd()final{SendEventEnd();}
};
class SignalPolinomialDistort:public Single2SingleSignal{
public:
	SignalPolinomialDistort(Vec&&coefs);
	virtual ~SignalPolinomialDistort();
	virtual void AcceptEventStart()final;
	virtual void AcceptSignalValue(double signal)final;
	virtual void AcceptEventEnd()final;
private:
	Vec m_coefs;
};
//There's a problem with transfering vector<smth>&& parameter as {val1,val2,...} to make_Shared template function
inline std::shared_ptr<SignalPolinomialDistort> PolynomDistort(Vec&&coefs){
	SignalPolinomialDistort *res=new SignalPolinomialDistort(static_right(coefs));
	return std::shared_ptr<SignalPolinomialDistort>(res);
}
inline std::shared_ptr<SignalPolinomialDistort> SignalAdd(double v){
	return PolynomDistort({v,1});
}
inline std::shared_ptr<SignalPolinomialDistort> SignalMultiply(double c){
	return PolynomDistort({0,c});
}
inline std::shared_ptr<SignalPolinomialDistort> SignalInvert(){
	return SignalMultiply(-1);
}
class SignalSmear:public Single2SingleSignal{
public:
	SignalSmear(double sigma);
	virtual ~SignalSmear();
	virtual void AcceptEventStart()final;
	virtual void AcceptSignalValue(double signal)final;
	virtual void AcceptEventEnd()final;
private:
	std::default_random_engine rnd;
	std::normal_distribution<double> smear;
};
/*class AmplitudeDiscriminator:public Single2SingleSignal{
public:
	AmplitudeDiscriminator(double);
};*/
class AbstractMultiInput:public std::enable_shared_from_this<AbstractMultiInput>{
	class Slot;
	friend class Slot;
public:
	AbstractMultiInput();
	virtual ~AbstractMultiInput();
	AbstractMultiInput&operator<<(std::shared_ptr<SignalProducent> input);
protected:
	virtual void Start()=0;
	virtual void Process(Vec&&signals)=0;
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
	virtual void Process(Vec&& signals)final;
};
class SignalProduct:public Multi2SingleSignal{
public:
	SignalProduct();
	virtual ~SignalProduct();
protected:
	virtual void Process(Vec&& signals)final;
};
class SignalSortAndSelect:public Multi2SingleSignal{
public:
	SignalSortAndSelect(size_t number);
	virtual ~SignalSortAndSelect();
protected:
	virtual void Process(Vec&&signals)final;
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
	virtual void Process(Vec&&signals)=0;
	virtual void Finish()final;
};
class TimeGate:public Multi2MultiSignal{
public:
    TimeGate(double width);
    virtual ~TimeGate();
protected:
    virtual void Process(Vec&& signals)override;
private:
	double m_width;
};
#endif 