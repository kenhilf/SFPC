#ifndef _actorparams_h_
#define _actorparams_h_

#include <string>
#include <vector>
#include "core.h"

class ActorParams
{
public:
	ActorParams();
	ActorParams(const std::string& name, const std::string& movetype, const std::string& ts, const int x, const int y);
	void Reset();
	void AddSpell(const std::string& spell);
	void AddItem(const std::string& item);

	std::string m_name;
	std::string m_charclass;
	std::string m_movetype;

	int m_level;
	int m_hp;
	int m_mp;
	int m_exp;
	int m_atk;
	int m_def;
	int m_agi;
	int m_mov;

	std::string m_tilesetName;
	std::string m_facesetName;

	std::string m_battlesetName;
	int m_numBattleFrames;

	int m_xpos, m_ypos;
	std::string m_facing;

	std::string m_spells[4];
	int m_numSpells;

	std::string m_items[4];
	int m_numItems;
	bool m_bEquipped[4];

	std::string m_team;

	std::vector<Direction::Type> m_steps;
};

#endif // _actorparams_h_