#ifndef _gametimer_h_
#define _gametimer_h_

#include "HighResTimer.h"
#include "windows.h"

namespace TimerMode
{
	enum Type { SysClock, RefreshRate, Fixed, NumTypes };
}

class GameTimer
{
public:
	GameTimer(const TimerMode::Type tm);

	void Reset();
	float GetPrimaryMonitorRefreshRate();
	float GetFrameTicks();
	void SetMaxFrameMS(const float frameMax);

private:
	double m_currSysElapsedMS;
	double m_lastSysElapsedMS;
	float m_frameTicks;
	float m_frameMax;

	TimerMode::Type m_timerMode;
	float m_refreshRateMsPerFrame;

public:
	// Tracker is the smart timer.  It does the rounding necesary for the movement code
	class Tracker
	{
	public:
		Tracker(const float unitsPerSec = 0) : m_elapsedTicks(0.0) { SetRate(unitsPerSec); }

		// Resets object
		void Reset() { m_elapsedTicks = 0.0; }

		// Initialize with rate in units/sec
		void SetRate(const float unitsPerSec);

		// Call to increment change in ticks
		void operator+=(const float ticks) { m_elapsedTicks += ticks; }

		// Call to determine number of units based on time elapsed.
		// If it returns a value > 0, which must be used because the internal
		// elapsed ticks will be reset.
		int GetUnitsAsInt();		

	private:
		float m_unitsPerTick;
		float m_elapsedTicks;
	};

	// Counter is the dumb timer.  It doesn't round, and is more useful for stuff that has rates that vary a lot
	class Counter
	{
	public:
		Counter() { m_elapsedTicks = 0.0;  }

		// Resets object
		void Reset() { m_elapsedTicks = 0.0; }

		// Call to increment change in ticks
		void operator+=(float ticks) { m_elapsedTicks += ticks; }

		float GetTicks() const { return m_elapsedTicks; }
		float GetMS()	 const { return m_elapsedTicks; }
		float GetSecs()  const { return m_elapsedTicks * 0.001f; }

	private:
		float m_elapsedTicks;
	};

};


#endif // _gametimer_h_
