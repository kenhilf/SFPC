#include "actorparams.h"
#include <assert.h>

ActorParams::ActorParams()
{
	Reset();
}

ActorParams::ActorParams(const std::string& name, const std::string& movetype, const std::string& ts, const int x, const int y)
{
	Reset();
	m_name = name;
	m_movetype = movetype;
	m_tilesetName = ts;
	m_xpos = x;
	m_ypos = y;
}

void ActorParams::Reset()
{
	m_name = "NOBODY";
	m_charclass = "NONE";
	m_movetype = "Flying";

	m_level = 1;
	m_hp = 1;
	m_mp = 0;
	m_exp = 0;
	m_atk = 1;
	m_def = 1;
	m_agi = 1;
	m_mov = 7;

	m_tilesetName = "sfhero.bmp";
	m_facesetName = "face_bowie.bmp";

	m_battlesetName = "None";
	m_numBattleFrames = 1;

	m_xpos = 10;
	m_ypos = 10;
	m_facing = "Down";

	m_spells[0] = "";
	m_spells[1] = "";
	m_spells[2] = "";
	m_spells[3] = "";
	m_numSpells = 0;

	m_items[0] = "";
	m_items[1] = "";
	m_items[2] = "";
	m_items[3] = "";
	m_bEquipped[0] = false;
	m_bEquipped[1] = false;
	m_bEquipped[2] = false;
	m_bEquipped[3] = false;
	m_numItems = 0;

	m_team = "NPC";

	m_steps.clear();
}

void ActorParams::AddSpell(const std::string& spell)
{
	assert(m_numSpells < 4);

	m_spells[m_numSpells] = spell;
	++m_numSpells;
}

void ActorParams::AddItem(const std::string& item)
{
	assert(m_numItems < 4);

	m_items[m_numItems] = item;
	++m_numItems;
}
