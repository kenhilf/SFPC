#include "magic.h"
#include <sstream>
#include "gamewindow.h"

//////////////////////
// MagicSpell		//
//////////////////////

MagicSpell::MagicSpell(const SpellName::Type spell, const int level, 
					   const int range, const int area) :
	m_spellName(spell),
	m_level(level),
	m_range(range),
	m_area(area)
{

}

const Texture& MagicSpell::GetTexture() const
{
	return GameWindowTexMgr::Instance().GetSpellIcon(m_spellName);
}

SpellName::Type MagicSpell::GetName() const
{
	return m_spellName;
}

int MagicSpell::GetLevel() const
{
	return m_level;
}

//////////////////////
// SpellBook		//
//////////////////////

SpellBook::SpellBook(const MagicSpell& spellone, const MagicSpell& spelltwo,
					 const MagicSpell& spellthree, const MagicSpell& spellfour)
{
	m_spells[0] = spellone;
	m_spells[1] = spelltwo;
	m_spells[2] = spellthree;
	m_spells[3] = spellfour;
}

void SpellBook::SetSpell(const MagicSpell& spell, const int slot)
{
	m_spells[slot] = spell;
}

SpellName::Type SpellBook::GetSpellName(const int slot) const
{
	return m_spells[slot].GetName();
}

int SpellBook::GetSpellLevel(const int slot) const
{
	return m_spells[slot].GetLevel();
}

const Texture& SpellBook::GetSpellTex(const int slot) const
{
	return m_spells[slot].GetTexture();
}

bool SpellBook::IsEmpty() const
{
	if ((m_spells[0].GetName() == SpellName::None) && 
		(m_spells[1].GetName() == SpellName::None) &&
		(m_spells[2].GetName() == SpellName::None) &&
		(m_spells[3].GetName() == SpellName::None))
		return true;
	else
		return false;
}

///////////////////////
// SpellBookRenderer //
///////////////////////

SpellBookRenderer::SpellBookRenderer() :
	GameObject(LayerSpace::Window),
	m_pSpellBook(NULL),
	m_vertical(false),
	m_textOnly(false)
{
	m_spellText.SetFont(FontMgr::Instance().GetFont(FontStyle::FixedWidth));
	m_spellText.SetOffset(80, -8);  // defaults to plus shaped config's text offsets
	AddChild(m_spellText);
}

void SpellBookRenderer::DoDraw(DrawMgr& drawMgr)
{
	if ((m_pSpellBook == NULL) || (m_textOnly))
		return;

	float x = GetWorldPos().x;
	float y = GetWorldPos().y;
	if (m_vertical)
	{
		for (int i = 0; i < 4; i++)
		{
			if (m_pSpellBook->GetSpellName(i) != SpellName::None)
			{
				drawMgr.DrawQuad(m_pSpellBook->GetSpellTex(i), Vector2f(x, y), DrawSpace::Screen);
				y+=24;
			}
		}
	}
	else // plus formation
	{
		drawMgr.DrawQuad(m_pSpellBook->GetSpellTex(0), Vector2f(x+24, y+0),  DrawSpace::Screen);
		drawMgr.DrawQuad(m_pSpellBook->GetSpellTex(1), Vector2f(x+0,  y+16), DrawSpace::Screen);
		drawMgr.DrawQuad(m_pSpellBook->GetSpellTex(2), Vector2f(x+48, y+16), DrawSpace::Screen);
		drawMgr.DrawQuad(m_pSpellBook->GetSpellTex(3), Vector2f(x+24, y+32), DrawSpace::Screen);
	}
}

void SpellBookRenderer::SetSpellBook(SpellBook* pSB)
{
	m_pSpellBook = pSB;
	if (m_pSpellBook)
	{
		std::stringstream temp;

		if (m_pSpellBook->IsEmpty())
		{
			if (m_vertical && !m_textOnly)
			{
				m_spellText.SetOffset(8, 8);
			}
			temp << "*1Nothing*0";
		}
		for (int i = 0; i < 4; i++)
		{
			if (m_pSpellBook->GetSpellName(i) != SpellName::None)
			{
				InsertSpellIntoStream(i, temp);
			}
		}
		m_spellText.SetString(temp.str());
	}
}

void SpellBookRenderer::SetRenderMode(const bool vert, const bool textmode)
{
	m_textOnly = textmode;
	m_vertical = vert;

	if (m_textOnly)
	{
		m_spellText.SetOffset(0, 0);
	}
	else if (m_vertical) // && !m_textOnly
	{
		m_spellText.SetOffset(17, 0);
	}
	else // !m_vertical && !m_textOnly
	{
		m_spellText.SetOffset(80, -8);
	}
}

void SpellBookRenderer::InsertSpellIntoStream(const int spellnum, std::stringstream& stream)
{
	if (m_pSpellBook == NULL)
		return;

	std::string spell = SpellName::Convert::EnumToName(m_pSpellBook->GetSpellName(spellnum));

	if (m_vertical && !m_textOnly)
	{
		stream << spell << "\n Lv." << m_pSpellBook->GetSpellLevel(spellnum) << "\n \n";
	}
	else
	{
		stream << spell << "\n Level " << m_pSpellBook->GetSpellLevel(spellnum) << "\n";
	}
}