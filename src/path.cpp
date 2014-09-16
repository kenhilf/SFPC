#include "path.h"

Path::Path() :
	m_bRepeating(false),
	m_nextStep(0)
{

}

Path::~Path()
{

}

void Path::AddStep(const Direction::Type dir)
{
	m_steps.push_back(Vector2f(dir));
}

void Path::AddStep(const Vector2f& vec)
{
	m_steps.push_back(vec);
}

Direction::Type Path::GetNextStepDir()
{
	assert(m_nextStep < m_steps.size());

	Direction::Type dir = Direction::None;
	if (m_steps[m_nextStep].y <= -1.0f)
		dir = Direction::Up;
	else if (m_steps[m_nextStep].y >= 1.0f)
		dir = Direction::Down;
	else if (m_steps[m_nextStep].x >= 1.0f)
		dir = Direction::Right;
	else if (m_steps[m_nextStep].y <= -1.0f)
		dir = Direction::Left;
	else
		assert(false && "Error getting next step from path, invalid Vector2f");

	++m_nextStep;
	return dir;
}

Vector2f Path::GetNextStep()
{
	assert(m_nextStep < m_steps.size());
	return m_steps[m_nextStep++];
}

void Path::SetRepeating(const bool repeat)
{
	m_bRepeating = repeat;
}

bool Path::IsRepeating() const
{
	return m_bRepeating;
}

void Path::ResetProgress()
{
	m_nextStep = 0;
}

bool Path::IsFinished() const
{
	if (m_nextStep == m_steps.size())
		return true;
	else
		return false;
}

void Path::Clear()
{
	m_nextStep = 0;
	m_steps.clear();
}