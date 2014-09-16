#ifndef _slider_h_
#define _slider_h_

#include "gameobject.h"

class Slider : public GameObject
{
public:
	Slider(const Vector2f& onscreen = Vector2f(0.0f, 0.0f),
		   const Vector2f& offscreen = Vector2f(0.0f, 0.0f),
		   const float movePixelsPerSec = 500.0f);
	virtual ~Slider();
	void SlideOntoScreen();
	void SlideOffScreen();
	void SlidePixels(const Direction::Type dir, const int numPixels);
	void SnapOntoScreen();
	void SnapOffScreen();
	void SetOffScreenPos(const Vector2f& pos);
	void SetOnScreenPos(const Vector2f& pos);
	void SetSlideRate(const float movePixelsPerSec);
	Vector2f GetOnScreenPos() const;
	bool AtDest() const;
	bool IsOffScreen() const;

	virtual void DoPhysics(const float ticks);

private:
	Vector2f m_onScreenPos;
	Vector2f m_offScreenPos;
	Vector2f m_destPos;
	Vector2f m_path;
	float m_velocity;
};

#endif //_slider_h_