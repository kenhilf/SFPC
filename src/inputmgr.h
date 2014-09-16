#ifndef _inputmgr_h_
#define _inputmgr_h_

#include <windows.h>
#include <winuser.h>
#include <map>
#include "core.h"
#include "vector2f.h"

typedef unsigned int InputEvent;
typedef int KeyType;

static InputEvent GEN_A = VK_SPACE;		// Confirm
static InputEvent GEN_B = 'C';			// Cancel
static InputEvent GEN_C = 'X';			// Menu

class InputMgr
{
// singleton privates - constructor, copy, and operator=
private:
	InputMgr();
	InputMgr(const InputMgr& rhs);
	InputMgr& operator=(const InputMgr& rhs);

public:
	// returns singleton instance
	static InputMgr& Instance() { static InputMgr instance;  return instance; };

	void Update();

	bool IsDown(const InputEvent ievent);
	bool IsUp(const InputEvent ievent) { return !IsDown(ievent); };
	bool WasPressed(const InputEvent ievent);
	bool WasReleased(const InputEvent ievent);

	Direction::Type GetDPadDirection() const;
	Vector2f GetDPadVector() const;
	
	void RegisterKey(const KeyType key) { m_registeredKeys[key] = KeyState(); }

private:
	struct KeyState
	{
		KeyState() { bIsDown = false; bWasDown = false; }

		bool bIsDown;
		bool bWasDown;  // last frame's IsDown
	};

	Direction::Type m_dPadDir;
	Vector2f m_dPadVect;

	typedef std::map<KeyType, KeyState> MapKeyToState;
	MapKeyToState m_registeredKeys;
};

#endif // _inputmgr_h_


