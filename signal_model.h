#ifndef CilPeTQJ
#define CilPeTQJ
#include <functional>
#include "math_h/sigma.h"
#include "math_h/interpolate.h"
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
	unsigned int events_count();
	void Reset();
private:
	unsigned int current;
	Sigma<double> m_count;
};
class ISignalChannel{
public:
	virtual void EventStarted()=0;
	virtual void Press(double time)=0;
	virtual void Release(double time)=0;
	virtual void EventFinished()=0;
};
class TimeDistribution:public ISignal{
public:
	TimeDistribution(double from,double to, int bins);
	virtual ~TimeDistribution();
	virtual void Start()override;
	virtual void Photon(double t)override;
	virtual void End()override;
	Distribution<double>&&GetDistribution();
private:
	Distribution<double> m_distr;
};
class AbstractSignalProducer{
public:
	virtual ~AbstractSignalProducer();
	AbstractSignalProducer&operator<<(std::shared_ptr<ISignalChannel>channel);
protected:
	void EventStarted();
	void Press(double time);
	void Release(double time);
	void EventFinished();
private:
	std::vector<std::shared_ptr<ISignalChannel>> m_channels;
};
class TimeSignal:public ISignal, public AbstractSignalProducer{
public:
	TimeSignal(unsigned int photon_count=0);
	virtual ~TimeSignal();
	virtual void Start()override;
	virtual void Photon(double t)override;
	virtual void End()override;
private:
	unsigned int current,m_photon_count;
};
typedef std::function<void()> Achtung;
class TimeSignalChannel:public ISignalChannel{
public:
	TimeSignalChannel(Achtung on_event_started_delegate=[](){},Achtung on_event_finished_delegate=[](){});
	virtual ~TimeSignalChannel();
	virtual void EventStarted()override;
	virtual void Press(double time)override;
	virtual void Release(double time)override;
	virtual void EventFinished()override;
	Pair&&Time();
	bool Active();
	bool InEvent();
private:
	bool m_state,in_event;
	Pair m_time;
	Achtung m_start,m_finish;
};
class AbstractTimeScheme{
public:
	AbstractTimeScheme();
	virtual ~AbstractTimeScheme();
	AbstractTimeScheme&operator<<(std::shared_ptr<AbstractSignalProducer> source);
	unsigned int Count();
	Pair&&LastTime(unsigned int index);
	bool LastActive(unsigned int index);
protected:
	virtual bool Condition()=0;
	virtual void Process()=0;
private:
	void Start_handler();
	void End_handler();
	int m_counter;
	std::vector<std::shared_ptr<TimeSignalChannel>> m_slots;
};
std::shared_ptr<AbstractTimeScheme> operator<<(std::shared_ptr<AbstractTimeScheme>,std::shared_ptr<AbstractSignalProducer>);
#endif