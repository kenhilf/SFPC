#ifndef _magic_h_
#define _magic_h_

#include "spellset.h"
#include "gameobject.h"
#include "gamestring.h"
#include <string>

class MagicSpell
{
public:
	MagicSpell(const SpellName::Type spell = SpellName::None, const int level = 1, 
			   const int range = 2, const int area = 1);
	const Texture& GetTexture() const;
	SpellName::Type GetName() const;
	int GetLevel() const;

private:
	SpellName::Type m_spellName;
	int m_level;
	int m_range;
	int m_area;
};

class SpellBook
{
public:
	SpellBook(const MagicSpell& spellone   = MagicSpell(SpellName::None), 
			  const MagicSpell& spelltwo   = MagicSpell(SpellName::None),
			  const MagicSpell& spellthree = MagicSpell(SpellName::None),
			  const MagicSpell& spellfour  = MagicSpell(SpellName::None));
	void SetSpell(const MagicSpell& spell, const int slot);
	SpellName::Type GetSpellName(const int slot) const;
	int GetSpellLevel(const int slot) const;
	const Texture& GetSpellTex(const int slot) const;
	bool IsEmpty() const;

private:
	MagicSpell m_spells[4];
};

class SpellBookRenderer : public GameObject
{
public:
	SpellBookRenderer();	
	virtual void DoDraw(DrawMgr& drawMgr);
	void SetSpellBook(SpellBook* pSB);
	void SetRenderMode(const bool vert = false, const bool textmode = false);

private:
	void InsertSpellIntoStream(const int spellnum, std::stringstream& stream);

	SpellBook* m_pSpellBook;
	bool m_vertical;
	bool m_textOnly;
	GameString m_spellText;
};

#endif //_magic_h_