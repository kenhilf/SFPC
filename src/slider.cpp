#include "slider.h"

Slider::Slider(const Vector2f& onscreen, const Vector2f& offscreen, const float movePixelsPerSec) :
	m_onScreenPos(onscreen),
	m_offScreenPos(offscreen),
	m_destPos(0.0f, 0.0f),
	m_velocity(movePixelsPerSec/1000.0f)
{
	SetLocalPos(onscreen);
}

Slider::~Slider()
{

}

void Slider::SlideOntoScreen()
{
	m_destPos = m_onScreenPos;
}

void Slider::SlideOffScreen()
{
	m_destPos = m_offScreenPos;
}

void Slider::SlidePixels(const Direction::Type dir, const int numPixels)
{
	Vector2f pos = GetLocalPos();
	if (dir == Direction::Up)
	{
		pos.y -= numPixels;
		m_destPos = pos;
//		m_slidePath = Line(GetLocalPos().x, GetLocalPos().y, m_destPos.x, m_destPos.y);
//		m_slideTimeTracker.Reset();	
	}
	else if (dir == Direction::Down)
	{
		pos.y += numPixels;
		m_destPos = pos;
//		m_slidePath = Line(GetLocalPos().x, GetLocalPos().y, m_destPos.x, m_destPos.y);
//		m_slideTimeTracker.Reset();	
	}
}

void Slider::SnapOntoScreen()
{
	SetLocalPos(m_onScreenPos);
	m_destPos = m_onScreenPos;
}

void Slider::SnapOffScreen()
{
	SetLocalPos(m_offScreenPos);
	m_destPos = m_offScreenPos;
}

void Slider::SetOnScreenPos(const Vector2f& pos)
{
	m_onScreenPos = pos;	
}

void Slider::SetOffScreenPos(const Vector2f& pos)
{
	m_offScreenPos = pos;
}

void Slider::SetSlideRate(const float movePixelsPerSec)
{
	m_velocity = movePixelsPerSec/1000.0f;
}

Vector2f Slider::GetOnScreenPos() const
{
	return m_onScreenPos;
}

bool Slider::AtDest() const
{
	Vector2f pos = GetLocalPos();
	if (pos == m_destPos)
		return true;
	else
		return false;
}

bool Slider::IsOffScreen() const
{
	Vector2f pos = GetLocalPos(); 
	if (pos == m_offScreenPos)
		return true;
	else
		return false;
}

void Slider::DoPhysics(const float ticks)
{
	if (AtDest())
		return;

	m_path = GetLocalPos();
	m_path.x = m_destPos.x - m_path.x;
	m_path.y = m_destPos.y - m_path.y;

	float pathMag = m_path.Magnitude();
	if (!(pathMag < (m_velocity * ticks)))
		pathMag = (m_velocity * ticks);
	m_path.Normalize();
	m_path = m_path * pathMag;
	SetLocalPos(GetLocalPos() + m_path);
}
/*
	std::stringstream temp;
	temp << "CamX: " << m_camPos.x << "\n"
		 << "CamY: " << m_camPos.y << "\n"
		 << "ActX: " << m_actorPos.x << "\n"
		 << "ActY: " << m_actorPos.y << "\n"
		 << "TgtX: " << m_actorPos.x - ((GAME_SCREEN_WIDTH/2) - (TILE_WIDTH/2)) << "\n"
		 << "TgtY: " << m_actorPos.y - ((GAME_SCREEN_HEIGHT/2) - (TILE_HEIGHT/2)) << "\n"
		 << "PthX: " << path.x << "\n"
		 << "PthY: " << path.y << "\n";

	m_debugString.SetString(temp.str());

	if (m_path.IsZero()) 
	{
		m_camMode = CameraMode::FollowActor;
		m_bCameraLock = true;
	}	

	m_slideTimeTracker += ticks;

	int pixelsToMove = m_slideTimeTracker.GetUnitsAsInt();

	Vector2f pos = GetLocalPos();
	int i = 0;

	while ((m_slidePath.GetNextPoint(pos.x, pos.y)) && i < pixelsToMove)
	{
		++i;
	}

	SetLocalPos(pos);

}*/