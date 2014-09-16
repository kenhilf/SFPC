#ifndef _statblock_h_
#define _statblock_h_

#include <string>
#include <sstream>
#include "core.h"
#include "magic.h"
#include "item.h"

namespace TerrainWalker
{
	enum Type { Human, Elf, Dwarf, Centaur, Tank, Hovering, Flying, NumTypes };

	extern const char* Names[];

	typedef EnumNameConverterT<Type, NumTypes, Names> Convert;
}

class Stat
{
public:
	Stat(const int val = 0);
	int Curr() const;

	int base, wpnMod, accMod, buffMod;
};

class StatBlock
{
public:
	StatBlock();
	StatBlock(const std::string& name, const CharacterClass::Type charClass, 
			  const TerrainWalker::Type tw, const int lv, const int maxhp, const int maxmp, 
			  const int exp, const int att, const int def, const int agi, const int mov, 
			  const int kills = 0, const int defeats = 0);
	std::string GetStatWindowString();
	std::string GetKillsDefeatsString();
	std::string GetTeamRowString();
	std::string GetBasicStatsString();
	std::string GetMagicStatsString();
	std::string GetItemStatsString();
	std::string GetEquipHeaderString();
	std::string GetEquipStatsString(const Item* pItem);
	int GetBFHUDWidth();
	std::string GetBattlefieldHUDString();

	const std::string& GetName() const;
	CharacterClass::Type GetCharacterClass() const;
	int GetLevel() const;
	int GetCurrHP() const;
	int GetMovement() const;
	int GetAgility() const;
	int GetAttack() const;
	int GetDefense() const;
	std::vector<int> GetAttackRange();

	void RefreshStats();

	bool TakeDamage(const int dmg);
	float GetLandEffect(const Terrain::Type terrain) const;

	SpellBook* GetSpellBook();
	Inventory* GetInventory();
	void LearnSpell(const SpellName::Type spell);
	bool AddItem(const std::string& itemname);

	void AddKill();
	void AddDefeat();
	bool AddExp(const int exp);

	bool IsAlive() const;

private:
	void InsertStatIntoStream(const int stat, std::stringstream& stream);
	void InsertNameIntoColumn(std::stringstream& stream, const int colsize = 7);

	std::string m_characterName;
	CharacterClass::Type m_charClass;
	TerrainWalker::Type m_terrainEffect;
	int m_level;
	int m_currHP, m_currMP;
	Stat m_maxHP, m_maxMP;
	Stat m_attack;
	Stat m_defense;
	Stat m_agility;
	Stat m_movement;

	int m_experience;
	int m_kills, m_defeats;

	SpellBook m_spellbook;
	int m_spellsKnown;
	Inventory m_inventory;
	int m_itemsHeld;
};

#endif //_statblock_h_