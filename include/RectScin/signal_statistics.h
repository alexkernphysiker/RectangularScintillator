// this file is distributed under 
// MIT license
#ifndef mxhvHUgn
#define mxhvHUgn
#include <fstream>
#include <string>
#include <functional>
#include "../math_h/sigma.h"
#include "photon2signal.h"
#include "signal_processing.h"
namespace RectangularScintillator{
	using namespace std;
	using namespace MathTemplates;
	class SignalStatictics:public SignalAcceptor{
	public:
		SignalStatictics(const double error_scale=1);
		virtual ~SignalStatictics();
		virtual void AcceptEventStart()override;
		virtual void AcceptSignalValue(const double time)override;
		virtual void AcceptEventEnd()override;
		const Sigma<double>&data()const;
	private:
		Sigma<double> m_data;
	};
	class SignalsToFile:public AbstractMultiInput{
	public:
		SignalsToFile();
		virtual ~SignalsToFile();
		void Redirect(const string&&name);
	protected:
		virtual void Start()override;
		virtual void Process(const Vec&signals)override;
		virtual void Finish()override;
	private:
		ofstream file;
	};
	
	class SignalAnalyse:public SignalAcceptor{
	public:
		SignalAnalyse(const function<void(double)> f);
		virtual ~SignalAnalyse();
		virtual void AcceptEventStart()override;
		virtual void AcceptSignalValue(const double time)override;
		virtual void AcceptEventEnd()override;
	private:
		function<void(double)> func;
	};

	class SignalsAnalyse:public AbstractMultiInput{
	public:
		SignalsAnalyse(const function<void(const Vec&)> f);
		virtual ~SignalsAnalyse();
	protected:
		virtual void Start()override;
		virtual void Process(const Vec&signals)override;
		virtual void Finish()override;
	private:
		function<void(const Vec&)> fnc;
	};
};
#endif 