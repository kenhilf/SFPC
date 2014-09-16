#ifndef _gameengine_h_
#define _gameengine_h_

#include "drawmgr.h"
#include "gametimer.h"
#include "camera.h"
class Actor;

class GameEngine
{
public:
	static GameEngine& Instance() { static GameEngine instance; return instance; }

	void Initialize();
	void ProcessInput();
	bool GameLoop();
	void StartBattle();
	void EndBattle();
	void SetActive(const bool active);
	bool IsActive() const;
	void Shutdown();

private:
	// hide constructor for singleton pattern
	GameEngine();

	// disable copy constructor and =operator
	GameEngine(const GameEngine& rhs);
	GameEngine& operator=(const GameEngine& rhs);

	bool m_bIsActive;
	bool m_bQuit;

	Vector2f m_pos;

	GameTimer m_timer;
	float m_frameTicks;

	Actor* m_pActor;

	DrawMgr m_drawMgr;

	float m_elapsedTime;
};


#endif // _gameengine_h_