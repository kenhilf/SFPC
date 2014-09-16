#ifndef _spellset_h_
#define _spellset_h_

#include "core.h"
#include "texture.h"
#include <string>


namespace SpellName
{
	enum Type { None, Apollo, Atlas, Attack, Aura, Blast, Blaze, Bolt, Boost, Dao, Desoul, Detox, Dispell, Egress, Freeze, Heal, Muddle, Neptun, Sleep, Slow, NumTypes };

	extern const char* Names[];

	typedef EnumNameConverterT<Type, NumTypes, Names> Convert;
}

class SpellSet
{
public:
	SpellSet(const std::string& filename);
	const Texture& GetSpellIcon(const SpellName::Type icon) const;

private:
	void LoadSpellSet(const std::string& filename);

	Texture m_spellIcons[SpellName::NumTypes];
};

#endif //_spellset_h_