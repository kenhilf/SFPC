#include "core.h"

namespace Direction
{
	const char* Names[] = { "Down", "Up", "Left", "Right", "None" };

	CT_ASSERT(sizeof(Names)/sizeof(Names[0]) == NumTypes);
}

Direction::Type Direction::Opposite(Direction::Type dir)
{
	Direction::Type opp;

	if (dir == Direction::Up)
		opp = Direction::Down;
	if (dir == Direction::Down)
		opp = Direction::Up;
	if (dir == Direction::Left)
		opp = Direction::Right;
	if (dir == Direction::Right)
		opp = Direction::Left;
	if (dir == Direction::None)
	{
		assert(false && "Direction::Opposite of None!");
		opp = Direction::None;
	}

	return opp;
}

namespace ActorTeam
{
	const char* Names[] = { "Player", "NPC", "Enemy", "Editor" };

	CT_ASSERT(sizeof(Names)/sizeof(Names[0]) == NumTypes);
}

ActorTeam::Type ActorTeam::Opponent(ActorTeam::Type opp)
{
	ActorTeam::Type team = ActorTeam::NumTypes;

	if (opp == ActorTeam::NPC)
		team = ActorTeam::Enemy;
	if (opp == ActorTeam::Player)
		team = ActorTeam::Enemy;
	if (opp == ActorTeam::Enemy)
		team = ActorTeam::Player;
	if (opp == ActorTeam::Editor)
		team = ActorTeam::Enemy;

	assert(team != ActorTeam::NumTypes);

	return team;
}

namespace CharacterClass
{
	const char* Names[] = 
	{
		"NONE", "SDMN", "PRST", "KNTE", "WARR", "MAGE", "THIF", "TORT", "PHNK", "RNGR", "WFMN", 
		"BDMN", "BRGN", "ACHR", "PLDN", "SORC", "BDBT", "VICR", "PGNT", "GLDT", "MMNK", 
		"RBT", "GLM", "WIZ", "RDBN", "HERO", "BRN", "NINJ", "MNST", "PHNX", "BWNT", 
		"WFBR", "SNIP", "ALL"
	};

	CT_ASSERT(sizeof(Names)/sizeof(Names[0]) == NumTypes);
}

namespace DamageElement
{
	const char* Names[] = 
	{
		"Physical", "Fire", "Cold", "Elec", "Wind", "Almighty"
	};

	CT_ASSERT(sizeof(Names)/sizeof(Names[0]) == NumTypes);
}

float EaseInOut(float aUnitValue)
{
    if (aUnitValue < 0.5f)
    {
        return 0.5f * Pow3(aUnitValue * 2.0f);
    }
    else
    {
        return 0.5f * (Pow3(aUnitValue * 2.0f - 2.0f) + 2.0f);
    }
}

/*
float easeIn(float t, float b, float c)
{
	return c*pow3(t) + b;
}
 
float easeOut(float t, float b, float c)
{
	return c*(pow3(t-1) + 1) + b;
}

float easeInOut(float t, float b, float c)
{
	float a = 0.0f;
	if ((t*2) < 1)
		a = c/2*pow3(t*2) + b;
	else
		a = c/2*(pow3(t*2-2) + 2) + b;

	return a;
}
*/