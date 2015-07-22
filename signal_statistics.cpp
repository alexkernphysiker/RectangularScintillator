// this file is distributed under 
// GPL v 3.0 license
#include "signal_statistics.h"
TimeSignalStatictics::TimeSignalStatictics(){}
TimeSignalStatictics::~TimeSignalStatictics(){}
void TimeSignalStatictics::AcceptEventStart(){}
void TimeSignalStatictics::AcceptSignalTime(double time){
	AddValue(time);
}
void TimeSignalStatictics::AcceptEventEnd(){}