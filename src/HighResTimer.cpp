#include "HighResTimer.h"
#include <windows.h>

namespace 
{
	LARGE_INTEGER g_li64Count;

	double GetSysTicksPerMS()
	{
		LARGE_INTEGER timerFrequency;
		QueryPerformanceFrequency(&timerFrequency);
		return static_cast<double>(timerFrequency.QuadPart) * 0.001;
	}

	double GetSysElapsedTicks()
	{
		QueryPerformanceCounter(&g_li64Count);
		return static_cast<double>(g_li64Count.QuadPart);
	}
}

// All of this is calculate once at load time
double HighResTimer::ms_ticksPerMS = (::GetSysTicksPerMS());
double HighResTimer::ms_msPerTick = (1.0f / ::GetSysTicksPerMS());
double HighResTimer::ms_ticksPerSec = (::GetSysTicksPerMS() * 1000.0);
double HighResTimer::ms_secsPerTick = (1.0f / ::GetSysTicksPerMS() / 1000.0);

double HighResTimer::GetSysElapsedTicks()
{
	return ::GetSysElapsedTicks();
}
