#include "inputmgr.h"
#include <assert.h>

InputMgr::InputMgr() :
	m_dPadDir(Direction::None),
	m_dPadVect(0.0f, 0.0f)
{
	// Register movement keys here for proper functioning of virtual dpad
	RegisterKey(VK_UP);
	RegisterKey(VK_DOWN);
	RegisterKey(VK_LEFT);
	RegisterKey(VK_RIGHT);
}

void InputMgr::Update()
{
	MapKeyToState::iterator iter = m_registeredKeys.begin();
	for ( ; iter != m_registeredKeys.end(); ++iter) 
	{
		KeyState& keyState = iter->second;
		keyState.bWasDown = keyState.bIsDown;
		keyState.bIsDown = (::GetAsyncKeyState(iter->first) & 0x8000) != 0;
	}

	if		(IsDown(VK_UP))		m_dPadDir = Direction::Up;
	else if (IsDown(VK_DOWN))	m_dPadDir = Direction::Down;
	else if (IsDown(VK_RIGHT))	m_dPadDir = Direction::Right;
	else if	(IsDown(VK_LEFT))	m_dPadDir = Direction::Left;
	else	/* (No Input) */	m_dPadDir = Direction::None;

	m_dPadVect.x = 0.0f;
	m_dPadVect.y = 0.0f;
	if		(IsDown(VK_LEFT))	m_dPadVect.x = -1.0f;
	else if	(IsDown(VK_RIGHT))	m_dPadVect.x = 1.0f;
	else if	(IsDown(VK_UP))		m_dPadVect.y = -1.0f;
	else if (IsDown(VK_DOWN))	m_dPadVect.y = 1.0f;
}

bool InputMgr::IsDown(const InputEvent ievent)
{
	MapKeyToState::iterator iter = m_registeredKeys.find(ievent);
	assert(iter != m_registeredKeys.end());
	return iter->second.bIsDown; 
}

bool InputMgr::WasPressed(const InputEvent ievent)
{
	MapKeyToState::iterator iter = m_registeredKeys.find(ievent);
	assert(iter != m_registeredKeys.end());
	return ((iter->second.bIsDown == true) && (iter->second.bWasDown == false));
}

bool InputMgr::WasReleased(const InputEvent ievent)
{
	MapKeyToState::iterator iter = m_registeredKeys.find(ievent);
	assert(iter != m_registeredKeys.end());
	return ((iter->second.bIsDown == false) && (iter->second.bWasDown == true));
}

Direction::Type InputMgr::GetDPadDirection() const
{
	return m_dPadDir;
}

Vector2f InputMgr::GetDPadVector() const
{
	return m_dPadVect;
}
