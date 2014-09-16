#include "statblock.h"
#include "itemdb.h"

namespace TerrainWalker
{
	const char* Names[] = 
	{
		"Human", "Elf", "Dwarf", "Centaur", "Tank", "Hovering", "Flying"
	};

	CT_ASSERT(sizeof(Names)/sizeof(Names[0]) == NumTypes);
}

//////////////
// Stat		//
//////////////

Stat::Stat(const int val) :
	base(val),
	wpnMod(0),
	accMod(0),
	buffMod(0)
{
	
}

int Stat::Curr() const
{
	return (base + wpnMod + accMod + buffMod);
}

///////////////
// StatBlock //
///////////////

StatBlock::StatBlock() :
	m_characterName("Default"),
	m_charClass(CharacterClass::SDMN),
	m_terrainEffect(TerrainWalker::Flying),
	m_level(1),
	m_maxHP(1),
	m_maxMP(0),
	m_attack(1),
	m_defense(1),
	m_agility(1),
	m_experience(0),
	m_movement(6),
	m_kills(0),
	m_defeats(0),
	m_spellsKnown(0),
	m_itemsHeld(0)
{
	m_currHP = m_maxHP.base;
	m_currMP = m_maxMP.base;
}

StatBlock::StatBlock(const std::string& name, const CharacterClass::Type charClass, 
					 const TerrainWalker::Type tw, const int lv, const int maxhp, 
					 const int maxmp, const int exp, const int att, const int def, const int agi, 
					 const int mov, const int kills, const int defeats) :
	m_characterName(name),
	m_charClass(charClass),
	m_terrainEffect(tw),
	m_level(lv),
	m_maxHP(maxhp),
	m_maxMP(maxmp),
	m_attack(att),
	m_defense(def),
	m_agility(agi),
	m_experience(exp),
	m_movement(mov),
	m_kills(kills),
	m_defeats(defeats),
	m_spellsKnown(0),
	m_itemsHeld(0)
{
	m_currHP = m_maxHP.base;
	m_currMP = m_maxMP.base;
}

std::string StatBlock::GetStatWindowString()
{
	std::stringstream temp;
	temp << CharacterClass::Convert::EnumToName(m_charClass) << " " << m_characterName.c_str() << "\n \n";
	temp << " LV    ";
	InsertStatIntoStream(m_level, temp);
	temp << "  ATT  ";
	InsertStatIntoStream(m_attack.Curr(), temp);
	temp << "\n \n";
	temp << " HP ";
	InsertStatIntoStream(m_currHP, temp);
	temp << "/";
	InsertStatIntoStream(m_maxHP.Curr(), temp);
	temp << "  DEF  ";
	InsertStatIntoStream(m_defense.Curr(), temp);
	temp << "\n \n";
	temp << " MP ";
	InsertStatIntoStream(m_currMP, temp);
	temp << "/";
	InsertStatIntoStream(m_maxMP.Curr(), temp);
	temp << "  AGI  ";
	InsertStatIntoStream(m_agility.Curr(), temp);
	temp << "\n \n";
	temp << " EXP   ";
	InsertStatIntoStream(m_experience, temp);
	temp << "  MOV  ";
	InsertStatIntoStream(m_movement.Curr(), temp);
	temp << "\n \n \n";
	temp << "MAGIC    ITEM\n";

	return temp.str();
}

std::string StatBlock::GetKillsDefeatsString()
{
	std::stringstream temp;

	temp << "\n \n \n \n \nKILLS\n    ";
	InsertStatIntoStream(m_kills, temp);
	temp << "\n \nDEFEAT\n    ";
	InsertStatIntoStream(m_defeats, temp);

	return temp.str();
}

std::string StatBlock::GetTeamRowString()
{
	std::stringstream temp;

	InsertNameIntoColumn(temp);
	temp << " " << CharacterClass::Convert::EnumToName(m_charClass) << "       ";
	InsertStatIntoStream(m_level, temp);
	temp << "  ";
	InsertStatIntoStream(m_experience, temp);
	temp << "\n \n";

	return temp.str();
}

std::string StatBlock::GetBasicStatsString()
{
	std::stringstream temp;

	temp << m_characterName << " " << CharacterClass::Convert::EnumToName(m_charClass) << " ";
	temp << "L" << m_level << "\n \n";
	temp << "MAGIC     ITEM\n";

	return temp.str();
}

std::string StatBlock::GetMagicStatsString()
{
	std::stringstream temp;

	temp << m_characterName << " " << CharacterClass::Convert::EnumToName(m_charClass) << " ";
	temp << "L" << m_level << "\n \n";
	temp << " - MAGIC\n";

	return temp.str();
}

std::string StatBlock::GetItemStatsString()
{
	std::stringstream temp;

	temp << m_characterName << " " << CharacterClass::Convert::EnumToName(m_charClass) << " ";
	temp << "L" << m_level << "\n \n";
	temp << " - ITEM -\n";

	return temp.str();
}

std::string StatBlock::GetEquipHeaderString()
{
	std::stringstream temp;

	temp << m_characterName << " " << CharacterClass::Convert::EnumToName(m_charClass) << " ";
	temp << "L" << m_level << "\n \n";
	temp << " Equipment\n";

	return temp.str();
}

std::string StatBlock::GetEquipStatsString(const Item* pItem)
{
	std::stringstream temp;

	int atk = m_attack.base;
	int def = m_defense.base;
	int agi = m_agility.base;
	int mov = m_movement.base;

	if (pItem != NULL)
	{
		// if the item we're looking at is a weapon, add accessory values to base stats
		// so we can see the difference the proposed weapon will have on the total
		if (pItem->GetItemClass() == ItemClass::Weapon)
		{
			atk += m_attack.accMod;
			def += m_defense.accMod;
			agi += m_agility.accMod;
			mov += m_movement.accMod;
		}
		// if the item we're looking at is an accessory, add weapon values to base stats
		// so we can see the difference the proposed accessory will have on the total
		else // pItem->GetItemClass() == ItemClass::Accessory
		{
			atk += m_attack.wpnMod;
			def += m_defense.wpnMod;
			agi += m_agility.wpnMod;
			mov += m_movement.wpnMod;
		}

		atk += pItem->GetAtkMod();
		def += pItem->GetDefMod();
		agi += pItem->GetAgiMod();
		mov += pItem->GetMovMod();
	}

	temp << "\n ATT  ";
	InsertStatIntoStream(atk, temp);
	temp << "\n \n DEF  ";
	InsertStatIntoStream(def, temp);
	temp << "\n \n AGI  ";
	InsertStatIntoStream(agi, temp);
	temp << "\n \n MOV  ";
	InsertStatIntoStream(mov, temp);

	return temp.str();
}

int StatBlock::GetBFHUDWidth()
{
	std::stringstream temp;

	temp << m_characterName;
	if (m_charClass != CharacterClass::NONE)
		temp << " " << CharacterClass::Convert::EnumToName(m_charClass) << m_level;

	return temp.str().size();
}

std::string StatBlock::GetBattlefieldHUDString()
{
	std::stringstream temp;

	temp << m_characterName;
	if (m_charClass != CharacterClass::NONE)
		temp << " " << CharacterClass::Convert::EnumToName(m_charClass) << m_level;
	
	int winwidth = temp.str().size();
	int numspaces = 0;

	temp << "\n";

	if (m_maxHP.Curr() >= 10)
		numspaces = winwidth - 7;
	else // m_maxHP < 10
		numspaces = winwidth - 5;
	temp << "\nHP";
	for (int i = 0; i < numspaces; ++i)
		temp << " ";
	if ((m_maxHP.Curr() >= 10) && (m_currHP < 10))
		temp << " ";
	temp << m_currHP << "/" << m_maxHP.Curr() << "\n";

	if (m_maxMP.Curr() >= 10)
		numspaces = winwidth - 7;
	else // m_maxMP < 10
		numspaces = winwidth - 5;
	temp << "\nMP";
	for (int i = 0; i < numspaces; ++i)
		temp << " ";
	temp << m_currMP << "/" << m_maxMP.Curr() << "\n";

	return temp.str();
}

const std::string& StatBlock::GetName() const
{
	return m_characterName;
}

CharacterClass::Type StatBlock::GetCharacterClass() const
{
	return m_charClass;
}

int StatBlock::GetLevel() const
{
	return m_level;
}

int StatBlock::GetCurrHP() const
{
	return m_currHP;
}

int StatBlock::GetMovement() const
{
	return m_movement.Curr();
}

int StatBlock::GetAgility() const
{
	return m_agility.Curr();
}

int StatBlock::GetAttack() const
{
	return m_attack.Curr();
}

int StatBlock::GetDefense() const
{
	return m_defense.Curr();
}

std::vector<int> StatBlock::GetAttackRange()
{
	Item* pItem = m_inventory.GetEquippedWeapon();
	if (pItem)
		return pItem->GetRange();
	else
	{
		std::vector<int> v;
		v.push_back(1);
		return v;
	}
}

void StatBlock::RefreshStats()
{
	Item* pWpn = m_inventory.GetEquippedWeapon();
	Item* pAcc = m_inventory.GetEquippedAccessory();

	if (pWpn)
	{
		m_maxHP.wpnMod = pWpn->GetHPMod();
		m_maxMP.wpnMod = pWpn->GetMPMod();
		m_attack.wpnMod = pWpn->GetAtkMod();
		m_defense.wpnMod = pWpn->GetDefMod();
		m_agility.wpnMod = pWpn->GetAgiMod();
		m_movement.wpnMod = pWpn->GetMovMod();
	}

	if (pAcc)
	{
		m_maxHP.accMod = pAcc->GetHPMod();
		m_maxMP.accMod = pAcc->GetMPMod();
		m_attack.accMod = pAcc->GetAtkMod();
		m_defense.accMod = pAcc->GetDefMod();
		m_agility.accMod = pAcc->GetAgiMod();
		m_movement.accMod = pAcc->GetMovMod();
	}

	// refresh stats based on buffs/debuffs
	// ...
}

bool StatBlock::TakeDamage(const int dmg)
{
	m_currHP -= dmg;
	if (m_currHP <= 0)
	{
		m_currHP = 0;
		return true;
	}
	else
		return false;
}

float StatBlock::GetLandEffect(const Terrain::Type terrain) const
{
	switch(m_terrainEffect)
	{
		case TerrainWalker::Human:			// Standard humanoid two legged units
			switch(terrain)
			{
				case Terrain::Smooth:		return 0.0f;	break;
				case Terrain::Rough:		return 0.15f;	break;
				case Terrain::Forest:		return 0.3f;	break;
				case Terrain::Hills:		return 0.45f;	break;
				case Terrain::Water:		return 20.0f;	break;	// impassible
				case Terrain::Mountains:	return 20.0f;	break;  // impassible
				default:					return 0.0f;	break;
			}
		break;
		case TerrainWalker::Elf:			// Two legged units with an affinity for Forest
			switch(terrain)
			{
				case Terrain::Smooth:		return 0.0f;	break;
				case Terrain::Rough:		return 0.15f;	break;
				case Terrain::Forest:		return 0.15f;	break;
				case Terrain::Hills:		return 0.45f;	break;
				case Terrain::Water:		return 20.0f;	break;	// impassible
				case Terrain::Mountains:	return 20.0f;	break;  // impassible
				default:					return 0.0f;	break;
			}
		break;
		case TerrainWalker::Dwarf:			// Two legged units with an affinity for Hills
			switch(terrain)
			{
				case Terrain::Smooth:		return 0.0f;	break;
				case Terrain::Rough:		return 0.15f;	break;
				case Terrain::Forest:		return 0.3f;	break;
				case Terrain::Hills:		return 0.3f;	break;
				case Terrain::Water:		return 20.0f;	break;	// impassible
				case Terrain::Mountains:	return 20.0f;	break;  // impassible
				default:					return 0.0f;	break;
			}
		break;
		case TerrainWalker::Centaur:		// Four legged units that tend to have high
			switch(terrain)					// MOV but problems off-roading
			{
				case Terrain::Smooth:		return 0.0f;	break;
				case Terrain::Rough:		return 0.15f;	break;
				case Terrain::Forest:		return 0.3f;	break;
				case Terrain::Hills:		return 0.45f;	break;
				case Terrain::Water:		return 20.0f;	break;	// impassible
				case Terrain::Mountains:	return 20.0f;	break;  // impassible
				default:					return 0.0f;	break;
			}
		break;
		case TerrainWalker::Tank:			// Treaded vehicles and artillery that
			switch(terrain)					// often have problems off-roading
			{
				case Terrain::Smooth:		return 0.0f;	break;
				case Terrain::Rough:		return 0.15f;	break;
				case Terrain::Forest:		return 0.45f;	break;
				case Terrain::Hills:		return 0.45f;	break;
				case Terrain::Water:		return 20.0f;	break;	// impassible
				case Terrain::Mountains:	return 20.0f;	break;  // impassible
				default:					return 0.0f;	break;
			}
		break;
		case TerrainWalker::Hovering:		// Low flying winged units that can fly over
			switch(terrain)					// most obstacles, but not mountains
			{
				case Terrain::Smooth:		return 0.0f;	break;
				case Terrain::Rough:		return 0.0f;	break;
				case Terrain::Forest:		return 0.0f;	break;
				case Terrain::Hills:		return 0.0f;	break;
				case Terrain::Water:		return 0.0f;	break;
				case Terrain::Mountains:	return 20.0f;	break;  // impassible
				default:					return 0.0f;	break;
			}
		break;
		case TerrainWalker::Flying:			// High flying winged units that can soar 
			switch(terrain)					// over any and all land types
			{
				case Terrain::Smooth:		return 0.0f;	break;
				case Terrain::Rough:		return 0.0f;	break;
				case Terrain::Forest:		return 0.0f;	break;
				case Terrain::Hills:		return 0.0f;	break;
				case Terrain::Water:		return 0.0f;	break;
				case Terrain::Mountains:	return 0.0f;	break;
				default:					return 0.0f;	break;
			}
		break;
		default: return 0.0f; break;
	}
}

SpellBook* StatBlock::GetSpellBook()
{
	return &m_spellbook;
}

Inventory* StatBlock::GetInventory()
{
	return &m_inventory;
}

void StatBlock::LearnSpell(const SpellName::Type spell)
{
	if (m_spellsKnown < 4)
	{
		m_spellbook.SetSpell(spell, m_spellsKnown);
		m_spellsKnown++;
	}
}

bool StatBlock::AddItem(const std::string& itemname)
{
	if (m_itemsHeld < 4)
	{
		for (int i = 0; i < 4; ++i)
		{
			if (m_inventory.GetItemGfxID(i) == ItemGfxID::Nothing)
			{
				// empty inventory spot found, fill it
				Item item = ItemDB::Instance().GetItem(itemname);
				m_inventory.SetItem(item, i);
				m_itemsHeld++;
				return true;
			}
		}
	}

	// inventory is full
	return false;
}

void StatBlock::AddKill()
{
	m_kills++;
}

void StatBlock::AddDefeat()
{
	m_defeats++;
}

bool StatBlock::AddExp(const int exp)
{
	m_experience += exp;
	if (m_experience > 99)
	{
		// Level up!
		m_experience = 0;
		return true;
	}
	else
		return false;
}

bool StatBlock::IsAlive() const
{
	if (m_currHP > 0)
		return true;
	else
		return false;
}

void StatBlock::InsertStatIntoStream(const int stat, std::stringstream& stream)
{
	if (stat < 10)
		stream << " ";
	stream << stat;
}

void StatBlock::InsertNameIntoColumn(std::stringstream& stream, const int colsize)
{
	stream << " " << m_characterName;
	if ((int)m_characterName.length() < colsize)
	{
		int i = (int)m_characterName.length();
		while (i < colsize)
		{
			stream << " ";
			i++;
		}
	}
}
