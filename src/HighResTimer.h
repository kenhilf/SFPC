#ifndef _HIGH_RES_TIMER_H_
#define _HIGH_RES_TIMER_H_


// High-resolution timer class, can be used to measure very small
// time intervals.

class HighResTimer
{
public:
	// Returns total elapsed ticks since system startup
	static double GetSysElapsedTicks();

	// Conversion functions
	static double TicksPerMS()		{ return ms_ticksPerMS; }
	static double MSPerTick()		{ return ms_msPerTick; }

	static double TicksPerSec()		{ return ms_ticksPerSec; }
	static double SecsPerTick()		{ return ms_secsPerTick; }

private:
	HighResTimer(); // Not-constructable

	static double ms_ticksPerMS;
	static double ms_msPerTick;
	static double ms_ticksPerSec;
	static double ms_secsPerTick;
};


#endif // _HIGH_RES_TIMER_H_
