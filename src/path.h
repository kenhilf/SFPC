#ifndef _path_h_
#define _path_h_

#include <vector>
#include "vector2f.h"
#include "core.h"

class Path
{
public:
	Path();
	~Path();

	void AddStep(const Direction::Type dir);
	void AddStep(const Vector2f& vec);
	Direction::Type GetNextStepDir();
	Vector2f GetNextStep();

	void SetRepeating(const bool repeat);
	bool IsRepeating() const;

	void ResetProgress();
	bool IsFinished() const;

	void Clear();

private:
	bool m_bRepeating;
	std::vector<Vector2f> m_steps;
	unsigned int m_nextStep;
};

#endif //_path_h_