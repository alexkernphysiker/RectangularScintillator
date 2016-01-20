// this file is distributed under 
// MIT license
#ifndef mxhvHUgn
#define mxhvHUgn
#include <fstream>
#include <string>
#include "../math_h/sigma.h"
#include "../math_h/interpolate.h"
#include "photon2signal.h"
#include "signal_processing.h"
namespace RectangularScintillator{
	using namespace std;
	using namespace MathTemplates;
	class SignalStatictics:public SignalAcceptor{
	public:
		SignalStatictics();
		virtual ~SignalStatictics();
		virtual void AcceptEventStart()override;
		virtual void AcceptSignalValue(double time)override;
		virtual void AcceptEventEnd()override;
		Sigma<double>&data()const;
		void Clear();
	private:
		Sigma<double> m_data;
	};
	class SignalDistribution:public SignalAcceptor{
	public:
		SignalDistribution(double from, double to, int bincount);
		virtual ~SignalDistribution();
		virtual void AcceptEventStart()override;
		virtual void AcceptSignalValue(double time)override;
		virtual void AcceptEventEnd()override;
		Distribution<double>&data()const;
		void Clear();
	private:
		double f,t;int cnt;
		Distribution<double> m_data;
	};
	class SignalsToFile:public AbstractMultiInput{
	public:
		SignalsToFile();
		virtual ~SignalsToFile();
		void Redirect(string name);
	protected:
		virtual void Start()override;
		virtual void Process(const Vec&signals)override;
		virtual void Finish()override;
	private:
		ofstream file;
	};
};
#endif 