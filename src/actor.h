#ifndef _actor_h_
#define _actor_h_

#include "gametimer.h"
#include "gameobject.h"
#include "animation.h"
#include "gamestring.h"
#include "faceset.h"
#include "actorparams.h"
#include "path.h"
class Tileset;
class StatBlock;
class Vector2f;

namespace MovementState
{
	enum Type { Moving, StoppingNoInput, StoppingForEvent, Idle, NumTypes };
}

class Actor : public GameObject
{
public:
	Actor(const ActorParams& ap);
	virtual ~Actor();
	void SetTileset(const std::string& filename);
	void SetRate(const float movePixelsPerSec);
	void SetTilePos(const Position& pos);
	Position GetTilePos() const;

	Direction::Type GetFacing() const;
	void SetFacing(const Direction::Type facing);

	void SetTalkScriptNum(const int num);
	int GetTalkScriptNum() const;

	void ObeySolids();
	void IgnoreSolids();

	StatBlock& GetStatBlock();
	ActorTeam::Type GetTeam() const;
	void SetTeam(ActorTeam::Type team);
	const std::string& GetBattleset() const;

	const Texture& GetFaceTex(const FaceState::Type state = FaceState::EyesOpen) const;

	bool IsMoving() const;
	bool IsColliding(const Vector2f& pos, Vector2f& offset);

	virtual void DoUpdate(const float ticks);
	void DoPlayerMovement(const float ticks);
	void DoNPCMovement(const float ticks);
	virtual void DoPhysics(const float ticks);
	virtual void DoDraw(DrawMgr& drawMgr);
	virtual void ProcessInput();

	void DoAction();

	void ClearMovementState();
//	void SetMoveLock(const bool lock);
//	bool IsMoveLocked() const;

	void TeleportTo(const Position& dest);

	void SetFollowPath(bool follow);
	Path& GetPath();

private:
	void ClampPos(Vector2f& pos);
	Position GetUnitPosByDirection(const Direction::Type dir);
	bool IsOnTileBoundary();

	bool m_bObeySolids;
	bool m_bEventCheck;

	Direction::Type m_facing;
	MovementState::Type m_currMoveState;
	float m_maxSpeed;
	Vector2f m_velocity;

	Tileset* m_pTileset;
	Animation m_sprite[Direction::NumTypes];
	GameString m_debugString;
	StatBlock* m_pStatBlock;
	FaceSet* m_pFaceSet;
	std::string m_battlesetName;
	int m_numBattleFrames;

	int m_talkScriptNum;

	ActorTeam::Type m_team;
//	bool m_bMoveLock;
	Vector2f m_lastPos;

	bool m_bFollowPath;
	Path m_path;
	Vector2f m_pathingDpad;
};

typedef std::vector<Actor*> ActorList;

#endif //_actor_h_

