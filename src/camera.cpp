#include "camera.h"
#include "commandmgr.h"
#include "gamemodemgr.h"
#include "menumgr.h"
#include "actor.h"
#include "inputmgr.h"
#include "gameobjectmgr.h"
#include "tilemap.h"
#include <sstream>

Camera::Camera(const int x, const int y, const float movePixelsPerSec) :
	GameObject(LayerSpace::Top),
	m_camPos(x, y),
	m_actorPos(0.0f, 0.0f),
	m_startPos(0.0f, 0.0f),
	m_endPos(0.0f, 0.0f),
	m_bCameraLock(false),
	m_camMode(CameraMode::FollowActor),
	m_lastCamMode(m_camMode),
	m_velocity(movePixelsPerSec),
	m_distToTravel(0.0f),
	m_duration(0.0f),
	m_ratio(0.0f),
	m_elapsedTicks(0.0f),
	m_pCurrActor(NULL)
{
	m_debugString.SetDrawSpace(DrawSpace::Screen);
	AddChild(m_debugString);
}

Camera::~Camera()
{

}

void Camera::SetRate(const float movePixelsPerSec)
{
	m_velocity = movePixelsPerSec/1000.0f;
}

const Vector2f& Camera::GetPos() const
{
	return m_camPos;
}

void Camera::FollowNewActor(Actor* pActor)
{
	m_pCurrActor = pActor;
	m_actorPos = m_pCurrActor->GetWorldPos();
	m_camMode = CameraMode::SwitchToActor;
	SetUpSwitchToActor();
}

void Camera::SnapToActor(Actor* pActor)
{
	m_pCurrActor = pActor;
	m_actorPos = m_pCurrActor->GetWorldPos();

	m_camPos.x = m_actorPos.x - ((GAME_SCREEN_WIDTH/2) - (TILE_WIDTH/2));
	m_camPos.y = m_actorPos.y - ((GAME_SCREEN_HEIGHT/2) - (TILE_HEIGHT/2));

	ClampPos();

	m_camMode = CameraMode::FollowActor;
}

void Camera::SnapToActor()
{
	m_actorPos = m_pCurrActor->GetWorldPos();

	m_camPos.x = m_actorPos.x - ((GAME_SCREEN_WIDTH/2) - (TILE_WIDTH/2));
	m_camPos.y = m_actorPos.y - ((GAME_SCREEN_HEIGHT/2) - (TILE_HEIGHT/2));

	ClampPos();

	m_camMode = CameraMode::FollowActor;
}

Actor* Camera::GetCurrActor()
{
	return m_pCurrActor;
}

bool Camera::IsMoving() const
{
	return m_bCameraLock;
}

void Camera::DoPhysics(const float ticks)
{
	if (m_camMode == CameraMode::SwitchToActor)
	{
		m_bCameraLock = true;
		SwitchToActor(ticks);
	}
	if (!m_bCameraLock || m_camMode == CameraMode::FollowActor)
	{
		m_bCameraLock = false;
		m_camMode = CameraMode::FollowActor;
		FollowActor();
	}

	m_lastCamMode = m_camMode;
}

void Camera::ProcessInput()
{
	if (CommandMgr::Instance().HasActiveCommands() || 
		m_pCurrActor->GetTeam() == ActorTeam::Enemy)
		return;

	// Process input on the current actor
	m_pCurrActor->ProcessInput();

	if (GameModeMgr::GetMode() == GameMode::Field)
	{
		// Open the root out of combat menu
		if (InputMgr::Instance().WasPressed(GEN_C)) 
		{
			MenuMgr::Instance().AddFourDirMenu(ActionName::Member, ActionName::Item, ActionName::Search, ActionName::Magic);
		}
	}
}

void Camera::FollowActor()
{
	// has the actor's position changed since last we checked?
	if (m_actorPos != m_pCurrActor->GetWorldPos())
	{
		m_actorPos = m_pCurrActor->GetWorldPos();
		m_camPos.x = m_actorPos.x - ((GAME_SCREEN_WIDTH/2) - (TILE_WIDTH/2));
		m_camPos.y = m_actorPos.y - ((GAME_SCREEN_HEIGHT/2) - (TILE_HEIGHT/2));

		ClampPos();
	}
}

void Camera::SetUpSwitchToActor()
{
	m_elapsedTicks = 0.0f;
	m_ratio = 0.0f;

	m_startPos = m_camPos;
	m_endPos.x = m_actorPos.x - ((GAME_SCREEN_WIDTH/2) - (TILE_WIDTH/2));
	m_endPos.y = m_actorPos.y - ((GAME_SCREEN_HEIGHT/2) - (TILE_HEIGHT/2));

	Vector2f path(m_endPos.x - m_camPos.x, m_endPos.y - m_camPos.y);
	float pathMag = path.Magnitude();
//	if (m_lastCamMode != CameraMode::SwitchToActor)
//		m_distToTravel = pathMag;

	m_duration = pathMag / m_velocity;
}

void Camera::SwitchToActor(const float ticks)
{
//	if (!m_bCameraLock)
//		return;

	if (m_ratio < 1.0f)
	{
		m_elapsedTicks += ticks;
		m_ratio = m_elapsedTicks / m_duration;

		if (m_ratio > 1.0f)
			m_ratio = 1.0f;

		m_ratio = EaseInOut(m_ratio);

		m_camPos = GetInterpolatedPosition(m_ratio);

		ClampPos();
	}
	else
		m_camMode = CameraMode::FollowActor;
/*
	std::stringstream temp;
	temp << "CamX: " << m_camPos.x << "\n"
		 << "CamY: " << m_camPos.y << "\n"
		 << "ActX: " << m_actorPos.x << "\n"
		 << "ActY: " << m_actorPos.y << "\n"
		 << "TgtX: " << m_actorPos.x - ((GAME_SCREEN_WIDTH/2) - (TILE_WIDTH/2)) << "\n"
		 << "TgtY: " << m_actorPos.y - ((GAME_SCREEN_HEIGHT/2) - (TILE_HEIGHT/2)) << "\n"
		 << "PthX: " << path.x << "\n"
		 << "PthY: " << path.y << "\n"
	  	 << "Vel : " << m_currVelocity << "\n"
		 << "Pct : " << pctDone;

	m_debugString.SetString(temp.str());
*/
}

void Camera::ClampPos()
{
	if (m_camPos.x < 0.0f)
		m_camPos.x = 0.0f;
	if (m_camPos.y < 0.0f)
		m_camPos.y = 0.0f;

	TileMap& tm = GameObjectMgr::Instance().GetTileMap();
//	float xLimit = tm.GetWidthInPixels() - ((GAME_SCREEN_WIDTH/2) - (TILE_WIDTH/2));
//	float yLimit = tm.GetHeightInPixels() - ((GAME_SCREEN_HEIGHT/2) - (TILE_HEIGHT/2));
	float xLimit = static_cast<float>(tm.GetWidthInPixels() - GAME_SCREEN_WIDTH - TILE_WIDTH - 1);
	float yLimit = static_cast<float>(tm.GetHeightInPixels() - GAME_SCREEN_HEIGHT - 1);

	if (m_camPos.x > xLimit)
		m_camPos.x = xLimit;

	if (m_camPos.y > yLimit)
		m_camPos.y = yLimit;
}

Vector2f Camera::GetInterpolatedPosition(float ratio)
{
	return m_startPos + (m_endPos - m_startPos) * ratio;
}
