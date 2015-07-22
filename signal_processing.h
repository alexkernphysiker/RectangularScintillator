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
inline std::shared_ptr<SignalPolinomialDistort> Distortion(Vec&&coefs){
	SignalPolinomialDistort *res=new SignalPolinomialDistort(static_right(coefs));
	return std::shared_ptr<SignalPolinomialDistort>(res);
}
class AbstractMultiInput{
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
		Slot(AbstractMultiInput*father);
        virtual ~Slot();
		virtual void AcceptEventStart()final;
		virtual void AcceptSignalValue(double time)final;
		virtual void AcceptEventEnd()final;
		double Value();
	private:
		AbstractMultiInput *master;
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
class SumWithWeights:public Multi2SingleSignal{
public:
	SumWithWeights(Vec&&weights);
	virtual ~SumWithWeights();
protected:
	virtual void Process(Vec&& signals)final;
private:
	Vec m_weights;
};
class ProductWithPowers:public Multi2SingleSignal{
public:
	ProductWithPowers(Vec&&powers);
	virtual ~ProductWithPowers();
protected:
	virtual void Process(Vec&& signals)final;
private:
	Vec m_powers;
};
class Single2MultiSignal{};
class Multi2MultiSignal{};
#endif 