#include "spellset.h"
#include "filepathmgr.h"

namespace SpellName
{
	const char* Names[] = { "None", "Apollo", "Atlas", "Attack", "Aura", "Blast", "Blaze",
							"Bolt", "Boost", "Dao", "Desoul", "Detox", "Dispell", "Egress",
							"Freeze", "Heal", "Muddle", "Neptun", "Sleep", "Slow" };

	CT_ASSERT(sizeof(Names)/sizeof(Names[0]) == NumTypes);
}

SpellSet::SpellSet(const std::string& filename)
{
	LoadSpellSet(filename);
}

const Texture& SpellSet::GetSpellIcon(const SpellName::Type icon) const
{
	return m_spellIcons[icon];
}

void SpellSet::LoadSpellSet(const std::string& filename)
{
	Image buffer;
	const std::string imagepath = FilePathMgr::Instance().GetImagePath() + filename;
	buffer.LoadBMP(imagepath.c_str());

	for (int i = 0; i < SpellName::NumTypes; i++)
	{
		m_spellIcons[i].LoadSubImage(buffer, (i*16), 0, 16, 24);
	}
}