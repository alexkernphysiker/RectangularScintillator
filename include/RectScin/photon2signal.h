// this file is distributed under 
// MIT license
#ifndef oRsHdaNh
#define oRsHdaNh
#include "../math_h/sigma.h"
#include "sensitive.h"
namespace RectangularScintillator{
	using namespace std;
	using namespace MathTemplates;
	class OrderStatisticsSigmaAnalyser:public PhotonTimeAcceptor{
	public:
		OrderStatisticsSigmaAnalyser(const size_t count);
		virtual ~OrderStatisticsSigmaAnalyser();
		const size_t count()const;
		const Sigma<double>& At(size_t i)const;
		virtual void AcceptEventStart()override;
		virtual void AcceptPhotonTime(const double time)override;
		virtual void AcceptEventEnd()override;
	private:
		size_t m_count;
		vector<Sigma<double>> m_stat;
	};
	class SignalAcceptor{
	public:
		virtual ~SignalAcceptor(){}
		virtual void AcceptEventStart()=0;
		virtual void AcceptSignalValue(const double signal)=0;
		virtual void AcceptEventEnd()=0;
	};
	class SignalProducent{
	public:
		virtual ~SignalProducent(){}
		SignalProducent&operator>>(const shared_ptr<SignalAcceptor>);
	protected:
		void SendEventStart();
		void SendSignalValue(const double time);
		void SendEventEnd();
	private:
		vector<shared_ptr<SignalAcceptor>> m_out_slots;
	};
	class WeightedTimeSignal:public PhotonTimeAcceptor,public SignalProducent{
	public:
		WeightedTimeSignal();
		virtual ~WeightedTimeSignal();
		WeightedTimeSignal&AddSummand(const size_t order_statistics,const double weight);
		virtual void AcceptEventStart()override;
		virtual void AcceptPhotonTime(const double time)override;
		virtual void AcceptEventEnd()override;
	private:
		vector<pair<size_t,double>> m_config;
		vector<bool> m_state;
		size_t m_count;
		double m_sum;
	};
	//There's a problem with transfering vector<smth>&& parameter as {val1,val2,...} to make_Shared template function
	inline shared_ptr<WeightedTimeSignal> TimeSignal(const vector<pair<size_t,double>>&&params){
		auto res=make_shared<WeightedTimeSignal>();
		for(auto p:params)
			res->AddSummand(p.first,p.second);
		return res;
	}
	class AmplitudeSignal:public PhotonTimeAcceptor,public SignalProducent{
	public:
		AmplitudeSignal();
		virtual ~AmplitudeSignal();
		virtual void AcceptEventStart()override;
		virtual void AcceptPhotonTime(const double time)override;
		virtual void AcceptEventEnd()override;
	private:
		size_t m_count;
	};
};
#endif 