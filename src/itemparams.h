#ifndef _itemparams_h_
#define _itemparams_h_

#include <string>
#include <vector>
#include "core.h"
#include "itemset.h"

namespace ItemClass
{
	//@TODO: refactor Tool to Consumable
	enum Type
	{
		Weapon, Accessory, Tool, Promotion, Quest, Nothing, NumTypes
	};

	extern const char* Names[];

	typedef EnumNameConverterT<Type, NumTypes, Names> Convert;
}

class ItemParams
{
public:
	ItemParams();
	ItemParams(const std::string& name);
	void Reset();

	std::string m_name;

	ItemClass::Type m_itemClass;

	ItemGfxID::Type m_itemGfxID;
	std::string m_cutsceneGfxFilename;

	std::vector<int> m_range;
	std::vector<CharacterClass::Type> m_equipable;

	int m_HPMod;
	int m_MPMod;
	int m_atkMod;
	int m_defMod;
	int m_agiMod;
	int m_movMod;

	int m_goldValue;

	bool m_bIsCursed;
	bool m_bIsBreakable;
	bool m_bIsCracked;
};

#endif //_itemparams_h_