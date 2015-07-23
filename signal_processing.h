// this file is distributed under 
// GPL v 3.0 license
#ifndef BYeAoSgG
#define BYeAoSgG
#include "photon2signal.h"
class SignalPolinomialDistort:public SignalAcceptor,public SignalProducent{
public:
	SignalPolinomialDistort(Vec&&coefs);
	virtual ~SignalPolinomialDistort();
	virtual void AcceptEventStart()final;
	virtual void AcceptSignalValue(double time)final;
	virtual void AcceptEventEnd()final;
private:
	Vec m_coefs;
};
inline std::shared_ptr<SignalPolinomialDistort> Distortion(Vec&&coefs){
	SignalPolinomialDistort *res=new SignalPolinomialDistort(static_right(coefs));
	return std::shared_ptr<SignalPolinomialDistort>(res);
}
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
		virtual void AcceptSignalValue(double time)final;
		virtual void AcceptEventEnd()final;
		double Value();
	private:
		std::shared_ptr<AbstractMultiInput>master;
		double m_value;
	};
	std::vector<std::shared_ptr<Slot>> m_input_slots;
	int m_state;
	std::shared_ptr<AbstractMultiInput> me;
};
class Multi2SingleSignal:public AbstractMultiInput,public SignalProducent{
public:
    Multi2SingleSignal();
	virtual ~Multi2SingleSignal();
protected:
	virtual void Start()final;
	virtual void Finish()final;
};
class SumWithWeights:public Multi2SingleSignal{
public:
	SumWithWeights(Vec&&weights);
	virtual ~SumWithWeights();
protected:
	virtual void Process(Vec&& signals)final;
private:
	Vec m_weights;
};
inline std::shared_ptr<SumWithWeights> SignalSum(Vec&&config){
	return std::shared_ptr<SumWithWeights>(new SumWithWeights(static_right(config)));
}
class ProductWithPowers:public Multi2SingleSignal{
public:
	ProductWithPowers(Vec&&powers);
	virtual ~ProductWithPowers();
protected:
	virtual void Process(Vec&& signals)final;
private:
	Vec m_powers;
};
inline std::shared_ptr<ProductWithPowers> SignalProduct(Vec&&config){
	return std::shared_ptr<ProductWithPowers>(new ProductWithPowers(static_right(config)));
}
class AbstractMultiOutput{
public:
	AbstractMultiOutput();
	virtual ~AbstractMultiOutput();
	AbstractMultiOutput&operator>>(std::shared_ptr<SignalAcceptor>output);
protected:
	void SendEventStart();
	size_t GetOutSlotsCount();
	void SendSignalValue(size_t i,double time);
	void SendEventEnd();
private:
	class Slot:public SignalProducent{
	public:
		Slot();
		virtual ~Slot();
		void Start();
		void Value(double time);
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
class SignalSorter:public Multi2MultiSignal{
public:
    SignalSorter();
    virtual ~SignalSorter();
protected:
    virtual void Process(Vec&& signals)final;
};
#endif 