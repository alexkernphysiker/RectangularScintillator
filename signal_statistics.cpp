// this file is distributed under 
// GPL v 3.0 license
#include "signal_statistics.h"
SignalStatictics::SignalStatictics(){}
SignalStatictics::~SignalStatictics(){}
void SignalStatictics::AcceptEventStart(){}
void SignalStatictics::AcceptSignalValue(double time){
	AddValue(time);
}
void SignalStatictics::AcceptEventEnd(){}