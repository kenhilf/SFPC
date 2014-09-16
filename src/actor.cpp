#include "actor.h"
#include "gameobjectmgr.h"
#include "inputmgr.h"
#include "windowmgr.h"
#include "commandmgr.h"
#include "filepathmgr.h"
#include "gamemodemgr.h"
#include "audiomgr.h"
#include "tileset.h"
#include "statblock.h"
#include "vector2f.h"
#include "blumath.h"
#include "collisioncheck.h"
#include <math.h>
#include "camera.h"
#include "menumgr.h"
#include "groundbattle.h"

Actor::Actor(const ActorParams& ap) :
	GameObject(LayerSpace::Overlay),
	m_bObeySolids(true),
	m_bEventCheck(true),
	m_facing(Direction::Down),
	m_currMoveState(MovementState::Idle),
	m_maxSpeed(0.096f),
	m_velocity(0.0f, 0.0f),
	m_pTileset(NULL),
	m_pStatBlock(NULL),
	m_pFaceSet(NULL),
	m_numBattleFrames(0),
	m_talkScriptNum(0),
	m_team(ActorTeam::NPC),
	m_bFollowPath(true)
//	m_bMoveLock(false)
{
	m_pFaceSet = new FaceSet(ap.m_facesetName);
	m_pStatBlock = new StatBlock(ap.m_name, 
								CharacterClass::Convert::NameToEnum(ap.m_charclass.c_str()),
								TerrainWalker::Convert::NameToEnum(ap.m_movetype.c_str()),
								ap.m_level, ap.m_hp, ap.m_mp, ap.m_exp,
								ap.m_atk, ap.m_def, ap.m_agi, ap.m_mov);

	m_facing = Direction::Convert::NameToEnum(ap.m_facing.c_str());
	m_team = ActorTeam::Convert::NameToEnum(ap.m_team.c_str());

	for (int i = 0; i < Direction::NumTypes; ++i)
		m_sprite[i].GetAnimationIndexer().Init(2, 250, AniType::Loop);

	SetTileset(ap.m_tilesetName);
	SetTilePos(Position(ap.m_xpos, ap.m_ypos));

	for (int i = 0; i < ap.m_numSpells; ++i)
		m_pStatBlock->LearnSpell(SpellName::Convert::NameToEnum(ap.m_spells[i].c_str()));

	for (int i = 0; i < ap.m_numItems; ++i)
	{
		m_pStatBlock->AddItem(ap.m_items[i].c_str());
		if (ap.m_bEquipped[i])
		{
			m_pStatBlock->GetInventory()->EquipWeapon(i);
			m_pStatBlock->RefreshStats();
		}
	}

	m_battlesetName = ap.m_battlesetName;
	m_numBattleFrames = ap.m_numBattleFrames;

	for (unsigned int i = 0; i < ap.m_steps.size(); i++)
	{
		m_path.AddStep(ap.m_steps[i]);
	}

	m_debugString.SetOffset(0, -24);
	m_debugString.SetScale(0.25f);
	m_debugString.SetDrawSpace(DrawSpace::Camera);
}

Actor::~Actor()
{
	if (m_pTileset != NULL)
		delete m_pTileset;

	if (m_pStatBlock != NULL)
		delete m_pStatBlock;

	if (m_pFaceSet != NULL)
		delete m_pFaceSet;
}

void Actor::SetTileset(const std::string& filename)
{
	m_pTileset = new Tileset(filename, 8, 1);

	Direction::Type currDir;
	for (int i = 0; i < 8; ++i)
	{
		switch(i)
		{
			case 0: currDir = Direction::Down;	m_sprite[currDir].SetTileset(m_pTileset); break;
			case 2: currDir = Direction::Up;	m_sprite[currDir].SetTileset(m_pTileset); break;
			case 4: currDir = Direction::Left;	m_sprite[currDir].SetTileset(m_pTileset); break;
			case 6: currDir = Direction::Right;	m_sprite[currDir].SetTileset(m_pTileset); break;
		}
		m_sprite[currDir].AddFrame(i);
	}
}

void Actor::SetRate(const float movePixelsPerSec)
{
	m_maxSpeed = movePixelsPerSec;
}

// does this need adjusted to play nice with the new movement code?
void Actor::SetTilePos(const Position& pos)
{
	TileMap& map = GameObjectMgr::Instance().GetTileMap();
	map.SetActorAtPos(GetWorldPos().CoordToTileNum(), NULL);
	map.SetActorAtPos(pos, this);
	SetLocalPos(Position(pos.x * TILE_WIDTH, pos.y * TILE_HEIGHT));
}

Position Actor::GetTilePos() const
{
	return GetWorldPos().CoordToTileNum();
}

Direction::Type Actor::GetFacing() const
{
	return m_facing;
}

void Actor::SetFacing(const Direction::Type facing)
{
	m_facing = facing;
}

void Actor::SetTalkScriptNum(const int num)
{
	m_talkScriptNum = num;
}

int Actor::GetTalkScriptNum() const
{
	return m_talkScriptNum;
}

void Actor::ObeySolids()
{
	m_bObeySolids = true;
}

void Actor::IgnoreSolids()
{
	m_bObeySolids = false;
}

StatBlock& Actor::GetStatBlock()
{
	return *m_pStatBlock;
}

ActorTeam::Type Actor::GetTeam() const
{
	return m_team;
}

void Actor::SetTeam(ActorTeam::Type team)
{
	m_team = team;
}

const std::string& Actor::GetBattleset() const
{
	return m_battlesetName;
}

const Texture& Actor::GetFaceTex(const FaceState::Type state) const
{
	return m_pFaceSet->GetFaceFrame(state);
}

bool Actor::IsMoving() const
{
	if (m_currMoveState != MovementState::Idle)
		return true; 
	else
		return false;
}

bool Actor::IsColliding(const Vector2f& pos, Vector2f& offset)
{
	if (!m_bObeySolids)
		return false;

	TileMap& map = GameObjectMgr::Instance().GetTileMap();
	Position tPos = pos.CoordToTileNum();
	Actor* pDestActor = NULL;

	for (int y = 0; y <= 1; ++y) 
	{
		tPos.y += y;
		for (int x = 0; x <= 1; ++x)
		{
			tPos.x += x;
			// collides with map solids?
			if ((map.IsPosSolid(tPos)) || (!map.IsPosFlashing(tPos)) )
			{
				Vector2f target(tPos.TileNumToCoord());
				if (CollisionCheck(pos, target, offset))
				{
					m_currMoveState = MovementState::Idle;
					return true;
				}
			}

			// collides with other actors?
			pDestActor = map.GetActorAtPos(tPos);
			bool bAllyPass = false;
			if (pDestActor != NULL)
			{
				if (pDestActor->GetTeam() == GetTeam())
					bAllyPass = true;
			}
			if (pDestActor && !bAllyPass)
			{
				Vector2f target(tPos.TileNumToCoord());
				if (CollisionCheck(pos, pDestActor->GetWorldPos(), offset))
				{
					m_currMoveState = MovementState::Idle;
					return true;
				}
			}
		}
		tPos.x -= 1;
	}

	return false;
}

void Actor::DoUpdate(const float ticks)
{
	for (int i = 0; i < 4; ++i)
	{
		m_sprite[i].DoUpdate(ticks);
	}

	// debugging map/actor relationship
//	assert(GameObjectMgr::Instance().GetTileMap().GetActorAtPos(GetWorldPos().CoordToTileNum())	== this);

	if ((GameObjectMgr::Instance().GetCamera().GetCurrActor() == this) &&
		((m_team == ActorTeam::Player) || (m_team == ActorTeam::Editor)))
		DoPlayerMovement(ticks);
	else
		DoNPCMovement(ticks);

/*	if (m_bObeySolids && 
		m_bEventCheck && 
		(m_currMoveState == MovementState::Idle) &&
		(m_lastPos != GetLocalPos()))
	{
		Position tilepos = GetWorldPos().CoordToTileNum();
		TileMap& map = GameObjectMgr::Instance().GetTileMap();
		int i = map.GetEventAtPos(tilepos);
		if (i > 0)
		{
			CommandMgr::Instance().LoadEventScript(i);
			m_bEventCheck = false;
			//@TEST: this might be bailing out of update too soon, may still have stuff to do
			return;
		}
	}

	m_velocity.x = 0.0f;
	m_velocity.y = 0.0f;
	const MovementState::Type lastMoveState = m_currMoveState;
	const Vector2f facingVector = DirToVector2f(m_facing);
	Vector2f dpad = InputMgr::Instance().GetDPadVector();
	// ignore the dpad if the current actor is an enemy, if the camera is transitioning to
	// a new actor, or if the targeting bracket is up in combat.
	if ((m_team == ActorTeam::Enemy) || 
		(GameObjectMgr::Instance().GetCamera().IsMoving()) ||
		(GroundBattle::Instance().IsTargeting()) ||
		(GroundBattle::Instance().IsCutsceneRunning()))
	{
		dpad.x = 0.0f;
		dpad.y = 0.0f;
	}
	Position tilepos = GetWorldPos().CoordToTileNum();
	TileMap& map = GameObjectMgr::Instance().GetTileMap();
	int eventnum = map.GetEventAtPos(tilepos);

	//@TODO: probably insufficient for when actors can move on their own, but should do for now
	if ((GameObjectMgr::Instance().GetCamera().GetCurrActor() == this) &&
		(!WindowMgr::Instance().HasActiveWindows()) &&
		(!MenuMgr::Instance().HasActiveMenus()))
	{
		switch(lastMoveState)
		{
			case MovementState::Idle:
			{
				if (!dpad.IsZero())
					m_currMoveState = MovementState::Moving;
			}
			break;

			case MovementState::StoppingNoInput:
			{
				if (eventnum > 0)
				{
					m_currMoveState = MovementState::StoppingForEvent;
				}
				else if (!dpad.IsZero())
				{
					if ((dpad.x == facingVector.x) && (dpad.y == facingVector.y))
						m_currMoveState = MovementState::Moving;
				}
				else if (IsOnTileBoundary())
					m_currMoveState = MovementState::Idle;
			}
			break;

			case MovementState::StoppingForEvent:
			{
				if (IsOnTileBoundary())
					m_currMoveState = MovementState::Idle;
			}
			break;

			case MovementState::Moving:
			{
				if (eventnum > 0)
				{
					m_currMoveState = MovementState::StoppingForEvent;
				}
				else if (dpad != facingVector)
					m_currMoveState = MovementState::StoppingNoInput;
			}
			break;
		}

		if (IsOnTileBoundary())
		{
			Direction::Type newfacing = Vector2fToDir(dpad);
			if (newfacing != Direction::None)
				m_facing = newfacing;
		}

		switch(m_currMoveState)
		{
			case MovementState::Idle:
			{
				m_velocity.x = 0.0f;
				m_velocity.y = 0.0f;
			}
			break;

			case MovementState::Moving:
			{
				m_velocity = DirToVector2f(m_facing) * m_maxSpeed * static_cast<float>(ticks);
			}
			break;

			case MovementState::StoppingNoInput:
			{
				// compute distance to travel
				m_velocity = DirToVector2f(m_facing) * m_maxSpeed * static_cast<float>(ticks);

				// do not travel more than one tile in case of large tick values
				m_velocity.x = Clamp(m_velocity.x, static_cast<float>(-TILE_WIDTH), static_cast<float>(TILE_WIDTH));
				m_velocity.y = Clamp(m_velocity.y, static_cast<float>(-TILE_WIDTH), static_cast<float>(TILE_WIDTH));
			}
			break;

			case MovementState::StoppingForEvent:
			{
				// compute distance to travel
				m_velocity = DirToVector2f(m_facing) * m_maxSpeed * static_cast<float>(ticks);

				// do not travel more than one tile in case of large tick values
				m_velocity.x = Clamp(m_velocity.x, static_cast<float>(-TILE_WIDTH), static_cast<float>(TILE_WIDTH));
				m_velocity.y = Clamp(m_velocity.y, static_cast<float>(-TILE_WIDTH), static_cast<float>(TILE_WIDTH));
			}
			break;
		}
	}
*/

/*	if (((pos.x % TILE_WIDTH)  == 0) &&
		((pos.y % TILE_HEIGHT) == 0) &&
		(m_bObeySolids) &&
		(m_bEventCheck))
	{
		TileMap& map = GameObjectMgr::Instance().GetTileMap();
		Position tilepos = GetWorldPos().CoordToTileNum();
		int i = map.GetEventAtPos(tilepos);
		if (i > 0)
		{
			m_remainingUnits = 0;
			CommandMgr::Instance().LoadEventScript(i);
			m_bEventCheck = false;
		}
	}
*/

	Vector2f textpos = GetWorldPos();
	Position debugtilepos = textpos.CoordToTileNum();
	std::stringstream temp;
	std::string name = m_pStatBlock->GetName();
	temp << "Pos: " << debugtilepos.x << "," << debugtilepos.y << "\n";
	temp << "Coord: " << GetLocalPos().x << ", " << GetLocalPos().y << "\n";
//	temp << "MS: " << m_currMoveState << "\n";
//	temp << "DPad: " << dpad.x << " " << dpad.y << "\n";
//	temp << "mVel: " << m_velocity.x << " " << m_velocity.y;

//	Terrain::Type terr = GameObjectMgr::Instance().GetTileMap().GetTerrainAtPos(tilepos);
//	float landeffect = m_pStatBlock->GetLandEffect(terr);
//	temp << "LE: " << landeffect;

	m_debugString.SetString(temp.str());
}

void Actor::DoPlayerMovement(const float ticks)
{
	if (m_bObeySolids && 
		m_bEventCheck && 
		(m_currMoveState == MovementState::Idle) &&
		(m_lastPos != GetLocalPos()))
	{
		Position tilepos = GetWorldPos().CoordToTileNum();
		TileMap& map = GameObjectMgr::Instance().GetTileMap();
		int i = map.GetEventAtPos(tilepos);
		if (i > 0)
		{
			CommandMgr::Instance().LoadEventScript(i);
			m_bEventCheck = false;
			//@TEST: this might be bailing out of update too soon, may still have stuff to do
			return;
		}
	}

	m_velocity.x = 0.0f;
	m_velocity.y = 0.0f;
	const MovementState::Type lastMoveState = m_currMoveState;
	const Vector2f facingVector = DirToVector2f(m_facing);
	Vector2f dpad = InputMgr::Instance().GetDPadVector();
	// ignore the dpad if the camera is transitioning to a new actor, or if the targeting 
	// bracket is up in combat.
	if ((GameObjectMgr::Instance().GetCamera().IsMoving()) ||
		(GroundBattle::Instance().IsTargeting()) ||
		(GroundBattle::Instance().IsCutsceneRunning()))
	{
		dpad.x = 0.0f;
		dpad.y = 0.0f;
	}
	Position tilepos = GetWorldPos().CoordToTileNum();
	TileMap& map = GameObjectMgr::Instance().GetTileMap();
	int eventnum = map.GetEventAtPos(tilepos);

	if ((!WindowMgr::Instance().HasActiveWindows()) &&
		(!MenuMgr::Instance().HasActiveMenus()))
	{
		switch(lastMoveState)
		{
			case MovementState::Idle:
			{
				if (!dpad.IsZero())
					m_currMoveState = MovementState::Moving;
			}
			break;

			case MovementState::StoppingNoInput:
			{
				if (eventnum > 0)
				{
					m_currMoveState = MovementState::StoppingForEvent;
				}
				else if (!dpad.IsZero())
				{
					if ((dpad.x == facingVector.x) && (dpad.y == facingVector.y))
						m_currMoveState = MovementState::Moving;
				}
				else if (IsOnTileBoundary())
					m_currMoveState = MovementState::Idle;
			}
			break;

			case MovementState::StoppingForEvent:
			{
				if (IsOnTileBoundary())
					m_currMoveState = MovementState::Idle;
			}
			break;

			case MovementState::Moving:
			{
				if (eventnum > 0)
				{
					m_currMoveState = MovementState::StoppingForEvent;
				}
				else if (dpad != facingVector)
					m_currMoveState = MovementState::StoppingNoInput;
			}
			break;
		}

		if (IsOnTileBoundary())
		{
			Direction::Type newfacing = Vector2fToDir(dpad);
			if (newfacing != Direction::None)
				m_facing = newfacing;
		}

		switch(m_currMoveState)
		{
			case MovementState::Idle:
			{
				m_velocity.x = 0.0f;
				m_velocity.y = 0.0f;
			}
			break;

			case MovementState::Moving:
			{
				m_velocity = DirToVector2f(m_facing) * m_maxSpeed * static_cast<float>(ticks);
			}
			break;

			case MovementState::StoppingNoInput:
			{
				// compute distance to travel
				m_velocity = DirToVector2f(m_facing) * m_maxSpeed * static_cast<float>(ticks);

				// do not travel more than one tile in case of large tick values
				m_velocity.x = Clamp(m_velocity.x, static_cast<float>(-TILE_WIDTH), static_cast<float>(TILE_WIDTH));
				m_velocity.y = Clamp(m_velocity.y, static_cast<float>(-TILE_WIDTH), static_cast<float>(TILE_WIDTH));
			}
			break;

			case MovementState::StoppingForEvent:
			{
				// compute distance to travel
				m_velocity = DirToVector2f(m_facing) * m_maxSpeed * static_cast<float>(ticks);

				// do not travel more than one tile in case of large tick values
				m_velocity.x = Clamp(m_velocity.x, static_cast<float>(-TILE_WIDTH), static_cast<float>(TILE_WIDTH));
				m_velocity.y = Clamp(m_velocity.y, static_cast<float>(-TILE_WIDTH), static_cast<float>(TILE_WIDTH));
			}
			break;
		}
	}
}

void Actor::DoNPCMovement(const float ticks)
{
/*
	// Assuming NPCs never trigger events for now
	if (m_bObeySolids && 
		m_bEventCheck && 
		(m_currMoveState == MovementState::Idle) &&
		(m_lastPos != GetLocalPos()))
	{
		Position tilepos = GetWorldPos().CoordToTileNum();
		TileMap& map = GameObjectMgr::Instance().GetTileMap();
		int i = map.GetEventAtPos(tilepos);
		if (i > 0)
		{
			CommandMgr::Instance().LoadEventScript(i);
			m_bEventCheck = false;
			//@TEST: this might be bailing out of update too soon, may still have stuff to do
			return;
		}
	}
*/
	m_velocity.x = 0.0f;
	m_velocity.y = 0.0f;
	const MovementState::Type lastMoveState = m_currMoveState;
	const Vector2f facingVector = DirToVector2f(m_facing);

	// ignore the dpad if the camera is transitioning to a new actor, or if the targeting 
	// bracket is up in combat.
/*	if ((GameObjectMgr::Instance().GetCamera().IsMoving()) ||
		(GroundBattle::Instance().IsTargeting()) ||
		(GroundBattle::Instance().IsCutsceneRunning()))
	{
		m_pathingDpad.x = 0.0f;
		m_pathingDpad.y = 0.0f;
	}
*/

//	if ((GameObjectMgr::Instance().GetCamera().GetCurrActor() == this) &&
//		(!WindowMgr::Instance().HasActiveWindows()) &&
//		(!MenuMgr::Instance().HasActiveMenus()))
	if ((!WindowMgr::Instance().HasActiveWindows()) &&
		(!MenuMgr::Instance().HasActiveMenus()))
	{
		switch(lastMoveState)
		{
			case MovementState::Idle:
			{
				//if (!m_pathingDpad.IsZero())
				if ((m_bFollowPath) && (!m_path.IsFinished()))
				{
					m_pathingDpad = m_path.GetNextStep();
					m_currMoveState = MovementState::Moving;

					//@TODO: need to set up something similar for player movement
					if (m_team == ActorTeam::Enemy)
						AudioMgr::PlaySoundEffect("dwoowoo.ogg");
				}
			}
			break;

			case MovementState::StoppingNoInput:
			{
/*				if (eventnum > 0)
				{
					m_currMoveState = MovementState::StoppingForEvent;
				}
				else if (!m_pathingDpad.IsZero())
				{
					if ((m_pathingDpad.x == facingVector.x) && (m_pathingDpad.y == facingVector.y))
						m_currMoveState = MovementState::Moving;
				}
				else */if (IsOnTileBoundary())
					m_currMoveState = MovementState::Idle;
			}
			break;

/*			case MovementState::StoppingForEvent:
			{
				if (IsOnTileBoundary())
					m_currMoveState = MovementState::Idle;
			}
			break;
*/
			case MovementState::Moving:
			{
/*				if (eventnum > 0)
				{
					m_currMoveState = MovementState::StoppingForEvent;
				}
				else if (m_pathingDpad != facingVector)*/
				m_currMoveState = MovementState::StoppingNoInput;
			}
			break;
		}

		if (IsOnTileBoundary())
		{
			Direction::Type newfacing = Vector2fToDir(m_pathingDpad);
			if (newfacing != Direction::None)
				m_facing = newfacing;
		}

		switch(m_currMoveState)
		{
			case MovementState::Idle:
			{
				m_velocity.x = 0.0f;
				m_velocity.y = 0.0f;
			}
			break;

			case MovementState::Moving:
			{
				m_velocity = DirToVector2f(m_facing) * m_maxSpeed * static_cast<float>(ticks);
			}
			break;

			case MovementState::StoppingNoInput:
			{
				// compute distance to travel
				m_velocity = DirToVector2f(m_facing) * m_maxSpeed * static_cast<float>(ticks);

				// do not travel more than one tile in case of large tick values
				m_velocity.x = Clamp(m_velocity.x, static_cast<float>(-TILE_WIDTH), static_cast<float>(TILE_WIDTH));
				m_velocity.y = Clamp(m_velocity.y, static_cast<float>(-TILE_WIDTH), static_cast<float>(TILE_WIDTH));
			}
			break;

			case MovementState::StoppingForEvent:
			{
				// compute distance to travel
				m_velocity = DirToVector2f(m_facing) * m_maxSpeed * static_cast<float>(ticks);

				// do not travel more than one tile in case of large tick values
				m_velocity.x = Clamp(m_velocity.x, static_cast<float>(-TILE_WIDTH), static_cast<float>(TILE_WIDTH));
				m_velocity.y = Clamp(m_velocity.y, static_cast<float>(-TILE_WIDTH), static_cast<float>(TILE_WIDTH));
			}
			break;
		}
	}
}

void Actor::DoPhysics(const float ticks)
{
	if (m_currMoveState == MovementState::Moving)
	{
		Vector2f off;
		Vector2f oldPos = GetLocalPos();
		m_lastPos = oldPos;
		Vector2f newPos = oldPos + m_velocity;

		if (IsColliding(newPos, off))
		{
			newPos += off;
		}

		if (m_bObeySolids)
		{
			TileMap& map = GameObjectMgr::Instance().GetTileMap();
			Actor* pSourceActor = map.GetActorAtCoord(oldPos);
			Actor* pDestActor   = map.GetActorAtCoord(newPos);
		
			if (pDestActor == NULL)
				map.SetActorAtCoord(newPos, this);
			if (pSourceActor == this)
				map.SetActorAtCoord(oldPos, NULL);
			m_bEventCheck = true;
		}		

		ClampPos(newPos);
		SetLocalPos(newPos);
	}
	else if ((m_currMoveState == MovementState::StoppingNoInput) ||
			 (m_currMoveState == MovementState::StoppingForEvent))
	{
		// use FMod to calculate local to world offset so we can work from the origin, and
		// FMod makes sure we're working with positive numbers.
		Vector2f oldPos = GetLocalPos();
		m_lastPos = oldPos;
		Vector2f localPos;
		localPos.x = FMod(oldPos.x, 0.0f, static_cast<float>(TILE_WIDTH));
		localPos.y = FMod(oldPos.y, 0.0f, static_cast<float>(TILE_WIDTH));
		Vector2f localToWorldOffset = oldPos - localPos;

		if (GetSign(m_velocity.x) > 0.0f)
			localPos.x = Min(localPos.x + m_velocity.x, static_cast<float>(TILE_WIDTH));
		else
			localPos.x = Max(localPos.x + m_velocity.x, 0.0f);

		if (GetSign(m_velocity.y) > 0.0f)
			localPos.y = Min(localPos.y + m_velocity.y, static_cast<float>(TILE_HEIGHT));
		else
			localPos.y = Max(localPos.y + m_velocity.y, 0.0f);

		Vector2f newPos = localToWorldOffset + localPos;
		Vector2f off;

		if (IsColliding(newPos, off))
		{
			newPos += off;
		}
		
		if (m_bObeySolids)
		{
			TileMap& map = GameObjectMgr::Instance().GetTileMap();
			Actor* pSourceActor = map.GetActorAtCoord(oldPos);
			Actor* pDestActor   = map.GetActorAtCoord(newPos);
		
			if (pDestActor == NULL)
				map.SetActorAtCoord(newPos, this);
			if (pSourceActor == this)
				map.SetActorAtCoord(oldPos, NULL);
			m_bEventCheck = true;
		}

		ClampPos(newPos);
		SetLocalPos(newPos);
	}
	else if (m_currMoveState == MovementState::Idle)
	{
		//@TODO: is this really necessary?  trying to fix bug with map not having 100% accurate
		// data on current actor positions, enemies ignoring players in combat being the most
		// annoying side effect of this but there may be others
		TileMap& map = GameObjectMgr::Instance().GetTileMap();
		map.SetActorAtCoord(GetWorldPos(), this);
	}

	//@TEMP: update debug textpos to make me less crazy and to help dispel visual noise of
	// debug text not being in sync with actor
	Vector2f textpos = GetWorldPos();
	m_debugString.SetWorldPos(Vector2f(textpos.x, textpos.y - 24.0f));
}

void Actor::DoDraw(DrawMgr& drawMgr)
{
	Vector2f pos = GetWorldPos();
	if (m_bObeySolids)
	{
		pos.y -= 8.0f;
	}

//	Actor* whoami = this;

	drawMgr.DrawQuad(m_sprite[m_facing].GetActiveFrame(), pos, DrawSpace::Camera);

	// conditionally draw the debug string only if we're in editing mode
	if (GameModeMgr::GetMode() == GameMode::EditMap)
		m_debugString.DoDraw(drawMgr);
}

void Actor::ProcessInput()
{
	if (GameObjectMgr::Instance().GetCamera().IsMoving())
		return;

	if (InputMgr::Instance().WasPressed(GEN_A))
		DoAction();
}

void Actor::DoAction()
{
	Position pos = GetWorldPos().CoordToTileNum();
	Position posCorrection = GetUnitPosByDirection(m_facing);
	pos += posCorrection;
	GameMode::Type mode = GameModeMgr::GetMode();

	TileMap& map = GameObjectMgr::Instance().GetTileMap();
	int i = map.GetEventAtPos(pos);
	Actor* pOtherActor = map.GetActorAtPos(pos);
	// If the tile we are looking at is solid, unoccupied, and has an event, run that event
	if ((map.IsPosSolid(pos) && m_bObeySolids) && !pOtherActor && i > 0)
	{
		CommandMgr::Instance().LoadEventScript(i);
	}
	// if there is another actor there, and we're not in combat, run their talk script
	// checking for m_bObeySolids is to keep the editor brackets from talking to actors.
	else if ((pOtherActor != NULL) && (pOtherActor != this) 
			  && (mode != GameMode::Battle) && m_bObeySolids)
	{
		pOtherActor->SetFacing(Direction::Opposite(m_facing));
		CommandMgr::Instance().LoadActorScript(pOtherActor->GetTalkScriptNum());
	}
}

void Actor::ClearMovementState()
{
	m_currMoveState = MovementState::Idle;
}

//@TODO: does this need adjusted to play nice with the new movement code?
void Actor::TeleportTo(const Position& dest)
{
	TileMap& map = GameObjectMgr::Instance().GetTileMap();
//	assert(map.GetActorAtPos(dest) == NULL);
	Position src = GetLocalPos().CoordToTileNum();
	map.SetActorAtPos(src, NULL);
	map.SetActorAtPos(dest, this);
	m_currMoveState = MovementState::Idle;
	SetLocalPos(dest.TileNumToCoord());
}

Path& Actor::GetPath()
{
	return m_path;
}

void Actor::ClampPos(Vector2f& pos)
{
	TileMap& map = GameObjectMgr::Instance().GetTileMap();

	if (pos.x < 0.0f)
		pos.x = 0.0f;

	if (pos.x > static_cast<float>((map.GetWidthInTiles() - 1) * TILE_WIDTH))
		pos.x = static_cast<float>((map.GetWidthInTiles() - 1) * TILE_WIDTH);

	if (pos.y < 0.0f)
		pos.y = 0.0f;

	if (pos.y > static_cast<float>((map.GetHeightInTiles() - 1) * TILE_HEIGHT))
		pos.y = static_cast<float>((map.GetHeightInTiles() - 1) * TILE_HEIGHT);
}

Position Actor::GetUnitPosByDirection(const Direction::Type dir)
{	
	switch (dir)
	{
		case Direction::Down:	return Position(0, 1);
		case Direction::Up:		return Position(0, -1);
		case Direction::Left:	return Position(-1, 0);
		case Direction::Right:	return Position(1, 0);
	}
	
	assert(false && "Invalid Direction::Type conversion in Actor::GetUnitPosByDirection");
	return Position();
}

bool Actor::IsOnTileBoundary()
{
	Vector2f pos = GetLocalPos();
	return ((fmodf(pos.x, static_cast<float>(TILE_WIDTH))  == 0.0f) && 
			(fmodf(pos.y, static_cast<float>(TILE_HEIGHT)) == 0.0f));
}

// old movement code:
/*
void Actor::Move(const double ticks)
{
	Vector2f direction;
	InputEvent key;
	switch(m_facing)
	{
		case Direction::Up:    key = VK_UP;		direction.y = -1.0f;	break;
		case Direction::Down:  key = VK_DOWN;	direction.y =  1.0f;	break;
		case Direction::Left:  key = VK_LEFT;	direction.x = -1.0f;	break;
		case Direction::Right: key = VK_RIGHT;	direction.x =  1.0f;	break;
	}

//	int dist = 0;//m_moveTracker.GetUnitsAsInt();

//	if (dist <= 0)
//		return;
//	if ((maxMoveUnits >= 0) && ((dist > maxMoveUnits) && (InputMgr::Instance().IsUp(key)) ))
//		dist = maxMoveUnits;


	Vector2f pos = GetLocalPos();
	float dist = static_cast<float>(m_maxSpeed * ticks);
	m_unitsMoved += dist;
	if (m_unitsMoved > m_unitsToMove)
	{
		dist -= (m_unitsMoved - m_unitsToMove);
		m_unitsMoved = 0.0f;
		m_unitsToMove = 0.0f;
	}
	direction *= dist;
	pos += direction;
	SetLocalPos(pos);
}

void Actor::MoveUnits(const float units)
{
	// if we're locking out movement (selecting targets, for instance) then bail
//	if (m_bMoveLock)
//		return;

	// only move if we're not already in motion
//	if (m_unitsMoved > m_unitsToMove)
//		return;
//	if (m_moveState != MovementState::Idle)
//		return;

	// test against tile we're trying to move into
	Vector2f pos = GetWorldPos();
	Position sourceTilePos = pos.CoordToTileNum();

	switch(m_facing)
	{
		case Direction::Up:    pos.y -= units; break;
		case Direction::Down:  pos.y += units; break;
		case Direction::Left:  pos.x -= units; break;
		case Direction::Right: pos.x += units; break;
	}
	ClampPos(pos);
	Position destTilePos = pos.CoordToTileNum();

	// after clamping pos, if the two are equal, we're at an edge, don't move.
	if (sourceTilePos == destTilePos)
		return;
	
	TileMap& map = GameObjectMgr::Instance().GetTileMap();

	Actor* pSourceActor = map.GetActorAtPos(sourceTilePos);
	Actor* pDestActor   = map.GetActorAtPos(destTilePos);

	// if there is an ally in the square we're trying to move through, they should
	// let us pass through, but not stop and execute an action in their square.
	bool bAllyPass = false;
	if (pDestActor != NULL)
	{
		if (pDestActor->GetTeam() == GetTeam())
			bAllyPass = true;
	}

	// if where we are trying to move is solid, occupied by an enemy or npc, or has not been 
	// flagged as in range this turn, face that direction but don't move in that direction 
	//@TEMP: IsPosSolid and IsPosFlashing currently also say no if asked if out of bounds
	// is not solid or walkable.
	if ((map.IsPosSolid(destTilePos) && m_bObeySolids) || 
		(pDestActor && m_bObeySolids && !bAllyPass) ||
		(!map.IsPosFlashing(destTilePos)) )
	{
		return;
	}

	// if all of the above passed, we can move
//	if (m_unitsToMove <= 0.0f)
	{
		if (GameModeMgr::GetMode() == GameMode::Battle)
			AudioMgr::PlaySoundEffect("dwoowoo.ogg");
		m_unitsToMove = units;
//		m_moveState = MovementState::Moving;
		if (m_bObeySolids)
		{
			if (pDestActor == NULL)
				map.SetActorAtPos(destTilePos, this);
			if (pSourceActor == this)
				map.SetActorAtPos(sourceTilePos, NULL);
			m_bEventCheck = true;
		}
	}
}
*/
