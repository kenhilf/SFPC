#include "fpscounter.h"

FPSCounter::FPSCounter() :
	GameObject(LayerSpace::Window),
	m_elapsedSeconds(0.0f),
	m_elapsedFrames(0.0f),
	m_fps(0.0f),
	m_avgfps(0.0f)
{
	m_output.SetDrawSpace(DrawSpace::Screen);
	AddChild(m_output);
}

FPSCounter::~FPSCounter()
{

}

void FPSCounter::DoUpdate(const float ticks)
{
	m_elapsedFrames++;
	m_elapsedSeconds += ticks;

	if (m_elapsedSeconds >= 1000.0f)
	{
		m_fps = m_elapsedFrames;
		m_avgfps = (m_avgfps + m_fps) / 2.0f;
		m_strbuff.str("");
		m_strbuff << "FPS: " << m_elapsedFrames << "\nAvg: " << m_avgfps << "\nFrameMS: " << ticks;
		m_output.SetString(m_strbuff.str());
		m_elapsedSeconds -= 1000.0f;
		m_elapsedFrames = 0.0f;
	}
}