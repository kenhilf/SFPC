#include "itemparams.h"

namespace ItemClass
{
	const char* Names[] = 
	{
		"Weapon", "Accessory", "Tool", "Promotion", "Quest", "Nothing"
	};

	CT_ASSERT(sizeof(Names)/sizeof(Names[0]) == NumTypes);
}

ItemParams::ItemParams()
{
	Reset();
}

ItemParams::ItemParams(const std::string& name)
{
	Reset();
	m_name = name;
}

void ItemParams::Reset()
{
	m_name = "Nothing";

	m_itemClass = ItemClass::Nothing;

	m_itemGfxID = ItemGfxID::Nothing;
	m_cutsceneGfxFilename = "None";

	m_range.clear();

	m_HPMod = 0;
	m_MPMod = 0;
	m_atkMod = 0;
	m_defMod = 0;
	m_agiMod = 0;
	m_movMod = 0;

	m_goldValue = 0;

	m_bIsCursed = false;
	m_bIsBreakable = false;
	m_bIsCracked = false;
}
