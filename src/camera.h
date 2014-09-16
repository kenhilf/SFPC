#ifndef _camera_h_
#define _camera_h_

#include "gameobject.h"
#include "vector2f.h"
#include "gamestring.h"
class Actor;

namespace CameraMode
{
	enum Type { SwitchToActor, FollowActor };
}

class Camera : public GameObject
{
public:
	Camera(const int x, const int y, const float movePixelsPerSec);
	virtual ~Camera();
	void SetRate(const float movePixelsPerSec);
	const Vector2f& GetPos() const;
	void FollowNewActor(Actor* pActor);
	void SnapToActor(Actor* pActor);
	void SnapToActor();
	Actor* GetCurrActor();
	bool IsMoving() const;

	virtual void DoPhysics(const float ticks);
	virtual void ProcessInput();

private:
	void FollowActor();
	void SetUpSwitchToActor();
	void SwitchToActor(const float ticks);
	void ClampPos();
	Vector2f GetInterpolatedPosition(float ratio);

	Vector2f m_camPos;
	Vector2f m_actorPos;
	Vector2f m_startPos;
	Vector2f m_endPos;

	bool m_bCameraLock;
	CameraMode::Type m_camMode;
	CameraMode::Type m_lastCamMode;

	float m_velocity;
	float m_distToTravel;
	float m_duration;
	float m_ratio;
	float m_elapsedTicks;

	Actor* m_pCurrActor;

	GameString m_debugString;
};

#endif //_camera_h_

