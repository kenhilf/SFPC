#include "gametimer.h"

//////////////////////
// GameTimer		//
//////////////////////

// Although it's not important externally, 1 TICK == 1 MS. Internally, it's
// important to store millseconds instead of ticks because there is less
// danger of millsecond values overflowing in a double than ticks (since it's
// less precise). In fact, all doubles could technically be replaced with floats,
// which would make it faster on 32-bit architectures.
// 5/12/11 - did just that and replaced double ticks with float ticks throughout the project

static double GetSysElapsedMS()
{
	return HighResTimer::GetSysElapsedTicks() * HighResTimer::MSPerTick();
}

GameTimer::GameTimer(const TimerMode::Type tm) :
	m_currSysElapsedMS(0.0),
	m_lastSysElapsedMS(0.0),
	m_frameTicks(0.0f),
	m_frameMax(100.0f),
	m_timerMode(tm),
	m_refreshRateMsPerFrame(0)
{
	if (m_timerMode == TimerMode::SysClock)
	{
		Reset();
	}
	else // m_timerMode == TimerMode::RefreshRate
	{
		float FramesPerSec = GetPrimaryMonitorRefreshRate(); // frames per sec
		m_refreshRateMsPerFrame = (1.0f / FramesPerSec) * 1000.0f;
	}
}

void GameTimer::Reset()
{
	m_currSysElapsedMS = GetSysElapsedMS();
	m_lastSysElapsedMS = GetSysElapsedMS();
	m_frameTicks = 0;
}

float GameTimer::GetPrimaryMonitorRefreshRate()
{
	DISPLAY_DEVICE dd;
	{
		int index=0;
		dd.cb = sizeof(DISPLAY_DEVICE);

		while (EnumDisplayDevices(NULL, index++, &dd, 0))
		{
			if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
				break;
		}
	}

	DEVMODE dm;
	dm.dmSize = sizeof(DEVMODE);

	if (!EnumDisplaySettings(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm))
	{
		return 0.0f;
	}

	return static_cast<float>(dm.dmDisplayFrequency);
}

float GameTimer::GetFrameTicks()
{
	if (m_timerMode == TimerMode::SysClock)
	{
		m_currSysElapsedMS = GetSysElapsedMS();
		const double timediff = m_currSysElapsedMS - m_lastSysElapsedMS;
		m_frameTicks = static_cast<float>(timediff);
		m_lastSysElapsedMS = m_currSysElapsedMS;

		if (m_frameTicks > m_frameMax)
			m_frameTicks = m_frameMax;

		return m_frameTicks;
	}
	else if (m_timerMode == TimerMode::RefreshRate)
	{
		return m_refreshRateMsPerFrame;
	}
	else // m_timerMode == TimerMode::Fixed
	{
		return 1.66667f;
	}
}

void GameTimer::SetMaxFrameMS(const float frameMax)
{
	m_frameMax = frameMax;
}

//////////////////////////
// GameTimer::Tracker	//
//////////////////////////

void GameTimer::Tracker::SetRate(const float unitsPerSec)
{
	m_unitsPerTick = unitsPerSec * 0.001f;
	m_elapsedTicks = 0;
}

int GameTimer::Tracker::GetUnitsAsInt()
{
	float unitsFlt = m_unitsPerTick * m_elapsedTicks;
	int unitsInt = static_cast<int>(unitsFlt + 0.5); // Round to nearest int value
	if (unitsInt <= 0)
		return 0;

	// Only keep remaining fractional part
	m_elapsedTicks = (unitsFlt - unitsInt) * (1.0f / m_unitsPerTick);
	return unitsInt;
}
