#include "item.h"
#include <algorithm>
#include "itemdb.h"
#include "filepathmgr.h"
#include "actor.h"
#include "statblock.h"
#include "gamewindow.h"

//////////////
// Item		//
//////////////

Item::Item()
{
	ItemParams defparams;
	Init(defparams);
}

Item::Item(const ItemParams& params)
{
	Init(params);
}

const Texture& Item::GetTexture() const
{
	return GameWindowTexMgr::Instance().GetItemIcon(m_itemGfxID);
}

ItemGfxID::Type Item::GetItemGfxID() const
{
	return m_itemGfxID;
}

const std::string& Item::GetName() const
{
	return m_name;
}

ItemClass::Type Item::GetItemClass() const
{
	return m_itemClass;
}

int Item::GetHPMod() const
{
	return m_HPMod;
}

int Item::GetMPMod() const
{
	return m_MPMod;
}

int Item::GetAtkMod() const
{
	return m_atkMod;
}

int Item::GetDefMod() const
{
	return m_defMod;
}

int Item::GetAgiMod() const
{
	return m_agiMod;
}

int Item::GetMovMod() const
{
	return m_movMod;
}

int Item::GetGoldValue() const
{
	return m_goldValue;
}

bool Item::CanEquip(CharacterClass::Type charclass) const
{
	bool equipable = false;
	std::vector<CharacterClass::Type>::const_iterator iter;

	iter = std::find(m_equipable.begin(), m_equipable.end(), CharacterClass::ALL);
	if (iter != m_equipable.end())
		equipable = true;

	iter = std::find(m_equipable.begin(), m_equipable.end(), charclass);
	if (iter != m_equipable.end())
		equipable = true;

	return equipable;
}

const std::vector<int>& Item::GetRange() const
{
	return m_range;
}

void Item::Init(const ItemParams& params)
{
	m_name = params.m_name;

	m_itemClass = params.m_itemClass;

	m_itemGfxID = params.m_itemGfxID;

	for (std::vector<int>::const_iterator iter = params.m_range.begin();
		 iter != params.m_range.end();
		 ++iter)
	{
		m_range.push_back(*iter);
	}

	for (std::vector<CharacterClass::Type>::const_iterator iter = params.m_equipable.begin();
		 iter != params.m_equipable.end();
		 ++iter)
	{
		m_equipable.push_back(*iter);
	}

	m_HPMod = params.m_HPMod;
	m_MPMod = params.m_MPMod;
	m_atkMod = params.m_atkMod;
	m_defMod = params.m_defMod;
	m_agiMod = params.m_agiMod;
	m_movMod = params.m_movMod;

	m_goldValue = params.m_goldValue;

	m_bIsCursed = params.m_bIsCursed;
	m_bIsBreakable = params.m_bIsBreakable;
	m_bIsCracked = params.m_bIsCracked;
}

//////////////////////
// Inventory		//
//////////////////////

Inventory::Inventory(const Item& itemone, const Item& itemtwo, 
					 const Item& itemthree, const Item& itemfour) :
	m_equippedWpnSlot(4),
	m_equippedAccSlot(4),
	m_pEquippedWeapon(NULL),
	m_pEquippedAccessory(NULL)
{
	m_items[0] = itemone;
	m_items[1] = itemtwo;
	m_items[2] = itemthree;
	m_items[3] = itemfour;
}

void Inventory::SetItem(const Item& item, const int slot)
{
	m_items[slot] = item;
}

ItemGfxID::Type Inventory::GetItemGfxID(const int slot) const
{
	assert((slot <= 4) && (slot >= 0));
	if (slot == 4)
		return ItemGfxID::Nothing;
	else
		return m_items[slot].GetItemGfxID();
}

const Texture& Inventory::GetItemTex(const int slot) const
{
	assert((slot <= 4) && (slot >= 0));
	if (slot == 4)
		return GameWindowTexMgr::Instance().GetItemIcon(ItemGfxID::Nothing);
	else
		return m_items[slot].GetTexture();
}

Item& Inventory::GetItem(const int slot)
{
	assert((slot <= 3) && (slot >= 0));
	return m_items[slot];
}

std::string Inventory::GetItemName(const int slot) const
{
	assert((slot <= 4) && (slot >= 0));
	if (slot == 4)
		return "*1Nothing*0";
	else
		return m_items[slot].GetName();
}

void Inventory::EquipWeapon(const int slot)
{
	if (m_items[slot].GetItemClass() == ItemClass::Weapon)
	{
		m_pEquippedWeapon = &m_items[slot];
		m_equippedWpnSlot = slot;
	}
	else // not a weapon, or we're deliberately unequipping, so equip nothing
	{
		m_pEquippedWeapon = NULL;
		m_equippedWpnSlot = 4;
	}
}

void Inventory::EquipAccessory(const int slot)
{
	if (m_items[slot].GetItemClass() == ItemClass::Accessory)
	{
		m_pEquippedAccessory = &m_items[slot];
		m_equippedAccSlot = slot;
	}
	else // not an accessory, or we're deliberately unequipping, so equip nothing
	{
		m_pEquippedAccessory = NULL;
		m_equippedAccSlot = 4;
	}
}

Item* Inventory::GetEquippedWeapon()
{
	return m_pEquippedWeapon;
}

Item* Inventory::GetEquippedAccessory()
{
	return m_pEquippedAccessory;
}

bool Inventory::IsEquipped(const int slot) const
{
	assert((slot <= 4) && (slot >= 0));
	if (slot == 4)
		return false;
	else if ((slot == m_equippedWpnSlot) || (slot == m_equippedAccSlot))
		return true;
	else
		return false;
}

//@TODO: check on item itself, not gfxid
bool Inventory::IsEmpty() const
{
	if ((m_items[0].GetItemGfxID() == ItemGfxID::Nothing) && 
		(m_items[1].GetItemGfxID() == ItemGfxID::Nothing) &&
		(m_items[2].GetItemGfxID() == ItemGfxID::Nothing) &&
		(m_items[3].GetItemGfxID() == ItemGfxID::Nothing))
		return true;
	else
		return false;
}

bool Inventory::HasAccessories() const
{
	if ((m_items[0].GetItemClass() == ItemClass::Accessory) || 
		(m_items[1].GetItemClass() == ItemClass::Accessory) ||
		(m_items[2].GetItemClass() == ItemClass::Accessory) ||
		(m_items[3].GetItemClass() == ItemClass::Accessory))
		return true;
	else
		return false;
}

///////////////////////
// InventoryRenderer //
///////////////////////

InventoryRenderer::InventoryRenderer() :
	GameObject(LayerSpace::Window),
	m_pInventory(NULL),
	m_pActor(NULL),
	m_vertical(false),
	m_textOnly(false),
	m_currChoice(Direction::Up),
	m_filter(InventoryFilter::All)
{
	m_itemText.SetFont(FontMgr::Instance().GetFont(FontStyle::FixedWidth));
	m_itemText.SetOffset(72, -16);  // defaults to text offset for plus shaped config
	AddChild(m_itemText);

	std::string imagepath = FilePathMgr::Instance().GetImagePath();
	Image temp;
	temp.LoadBMP(imagepath + "itembracket.bmp");
	m_iconBracket.LoadImage(temp);
	temp.LoadBMP(imagepath + "redbracket.bmp");
	m_nameBracket.LoadImage(temp);

	m_filteredInventory[0] = 4;
	m_filteredInventory[1] = 4;
	m_filteredInventory[2] = 4;
	m_filteredInventory[3] = 4;
}

InventoryRenderer::~InventoryRenderer()
{

}

void InventoryRenderer::DoDraw(DrawMgr& drawMgr)
{
	if ((m_pInventory == NULL) || (m_textOnly))
		return;

	float x = GetWorldPos().x;
	float y = GetWorldPos().y;

	if (m_vertical)
	{
		for (int i = 0; i < 4; i++)
		{
			if (m_pInventory->GetItemGfxID(i) != ItemGfxID::Nothing)
			{
				drawMgr.DrawQuad(m_pInventory->GetItemTex(i), Vector2f(x, y), DrawSpace::Screen);
				y+=24;
			}
		}
	}
	else // Plus formation
	{
		drawMgr.DrawQuad(m_pInventory->GetItemTex(m_filteredInventory[0]), Vector2f(x+24, y+0),  DrawSpace::Screen);
		drawMgr.DrawQuad(m_pInventory->GetItemTex(m_filteredInventory[1]), Vector2f(x+0,  y+16), DrawSpace::Screen);
		drawMgr.DrawQuad(m_pInventory->GetItemTex(m_filteredInventory[2]), Vector2f(x+48, y+16), DrawSpace::Screen);

		if (m_filteredInventory[3] == 4) // if last slot is empty, draw open hand instead
			drawMgr.DrawQuad(GameWindowTexMgr::Instance().GetItemIcon(ItemGfxID::Empty), Vector2f(x+24, y+32), DrawSpace::Screen);
		else // otherwise, the last slot has something to draw, so draw that
			drawMgr.DrawQuad(m_pInventory->GetItemTex(m_filteredInventory[3]), Vector2f(x+24, y+32), DrawSpace::Screen);

		if (m_filter != InventoryFilter::All)
		{
			Vector2f pos;
			switch(m_currChoice)
			{
				case Direction::Up:		pos = Vector2f(x+24, y+0);	break;
				case Direction::Left:	pos = Vector2f(x+0,  y+16);	break;
				case Direction::Right:	pos = Vector2f(x+48, y+16);	break;
				case Direction::Down:	pos = Vector2f(x+24, y+32);	break;
			}
			drawMgr.DrawQuad(m_iconBracket, pos, DrawSpace::Screen);
		}
	}
}

void InventoryRenderer::SetInventory(Inventory* pInv)
{
	m_pInventory = pInv;
	if (m_pInventory)
	{
		SetFilter(InventoryFilter::All);
		ShowItemList();
	}
}

void InventoryRenderer::SetInventoryHolder(Actor* pActor)
{
	m_pActor = pActor;
}

void InventoryRenderer::SetRenderMode(const bool vert, const bool textmode)
{
	m_textOnly = textmode;
	m_vertical = vert;

	if (m_textOnly)
	{
		m_itemText.SetOffset(0, 0);
	}
	else if (m_vertical) // && !m_textOnly
	{
		m_itemText.SetOffset(17, -8);
	}
	else // !m_vertical && !m_textOnly
	{
		m_itemText.SetOffset(72, -16);
	}
}

void InventoryRenderer::SetFilter(const InventoryFilter::Type filter)
{
	m_filter = filter;

	if (m_filter == InventoryFilter::All) // not filtering, map 1:1
	{
		m_filteredInventory[0] = 0;
		m_filteredInventory[1] = 1;
		m_filteredInventory[2] = 2;
		m_filteredInventory[3] = 3;
	}
	else // filtering by a ItemClass
	{
		ItemClass::Type classfilter;
		if (m_filter == InventoryFilter::Weapons)
			classfilter = ItemClass::Weapon;
		if (m_filter == InventoryFilter::Accessories)
			classfilter = ItemClass::Accessory;

		int numfilter = 0;
		for (int i = 0; i < 4; i++)
		{
			Item& item = m_pInventory->GetItem(i);
			if ((item.GetItemClass() == classfilter) &&
				(item.CanEquip(m_pActor->GetStatBlock().GetCharacterClass())))
			{
				m_filteredInventory[numfilter++] = i;
			}
		}

		// all other slots should show the Nothing icon (filter #4)
		while (numfilter < 4)
		{
			m_filteredInventory[numfilter++] = 4;
		}
	}
}

void InventoryRenderer::SetChoice(const Direction::Type dir)
{
	m_currChoice = dir;
}

void InventoryRenderer::ShowItemList()
{
	std::stringstream temp;

	if (m_pInventory->IsEmpty())
	{
		if (!m_vertical)
		{
			temp << " ";
		}
		if (m_vertical && !m_textOnly)
		{
			m_itemText.SetOffset(8, 8);
		}
		temp << "*1Nothing*0";
	}
	if (!m_textOnly)
		temp << "\n";
	for (int i = 0; i < 4; i++)
	{
		if (m_pInventory->GetItemGfxID(i) != ItemGfxID::Nothing)
		{
			InsertItemIntoStream(i, temp);
		}
	}

	m_itemText.SetString(temp.str());
}

void InventoryRenderer::ShowItemStats()
{
	std::stringstream temp;
	int i = ConvertDirToChoice(m_currChoice);
	InsertItemIntoStream(m_filteredInventory[i], temp);
	if (m_filteredInventory[i] == 4) // Get stats for nothing equipped in this slot
		temp << m_pActor->GetStatBlock().GetEquipStatsString(NULL);
	else	// Get stats for the filtered item
		temp << m_pActor->GetStatBlock().GetEquipStatsString(&m_pInventory->GetItem(m_filteredInventory[i]));
	m_itemText.SetString(temp.str());
}

void InventoryRenderer::EquipSelection()
{
	const int i = ConvertDirToChoice(m_currChoice);
	m_pInventory->EquipWeapon(m_filteredInventory[i]);
	m_pActor->GetStatBlock().RefreshStats();
}

void InventoryRenderer::InsertItemIntoStream(const int itemnum, std::stringstream& stream)
{
	assert(m_pInventory != NULL);

	// 1 space padding for E marker for equipped items
	if (!m_vertical)
	{
		if (m_pInventory->IsEquipped(itemnum))
			stream << "{";
		else
			stream << " ";
	}

	std::string item = m_pInventory->GetItemName(itemnum);

	const int numspaces = std::count(item.begin(), item.end(), ' ');
	if (numspaces == 0)
	{
		stream << item << "\n ";

		if (item == "*1Nothing*0")
			stream << "\n ";
	}
	else if (numspaces == 1)
	{
		int i = item.find(' ');
		if (!m_vertical)
		{
			item.insert(i, "\n ");
		}
		else
		{
			item.insert(i, "\n");
		}
		stream << item;
	}
	else if (numspaces == 2)
	{
		int i = item.find(' ');
		i = item.find(' ', i+1);
		if (!m_vertical)
		{
			item.insert(i, "\n ");
		}
		else
		{
			item.insert(i, "\n");
		}
		stream << item;
	}

	stream << "\n";
	if (m_vertical && !m_textOnly)
	{
		if (m_pInventory->IsEquipped(itemnum))
			stream << "*1Equipped*0";

		stream << " \n";
	}
}

int InventoryRenderer::ConvertDirToChoice(const Direction::Type dir)
{
	int i = -1;
	switch (m_currChoice)
	{
		case Direction::Up:		i = 0; break;
		case Direction::Left:	i = 1; break;
		case Direction::Right:	i = 2; break;
		case Direction::Down:	i = 3; break;
	}
	assert((i >= 0) && (i <= 3));
	return i;
}
