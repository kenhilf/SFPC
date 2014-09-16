#ifndef _item_h_
#define _item_h_

#include <string>
#include "itemset.h"
#include "itemparams.h"
#include "gameobject.h"
#include "gamestring.h"
class Actor;

namespace InventoryFilter
{
	enum Type { All, Weapons, Accessories, NumTypes };
}

class Item
{
public:
	Item();
	Item(const ItemParams& params);
	const Texture& GetTexture() const;
	ItemGfxID::Type GetItemGfxID() const;
	const std::string& GetName() const;
	ItemClass::Type GetItemClass() const;
	int GetHPMod() const;
	int GetMPMod() const;
	int GetAtkMod() const;
	int GetDefMod() const;
	int GetAgiMod() const;
	int GetMovMod() const;
	int GetGoldValue() const;
	bool CanEquip(CharacterClass::Type charclass) const;
	const std::vector<int>& GetRange() const;

private:
	void Init(const ItemParams& params);

	std::string m_name;

	ItemGfxID::Type m_itemGfxID;

	ItemClass::Type m_itemClass;

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

class Inventory
{
public:
	Inventory(const Item& itemone   = Item(), 
			  const Item& itemtwo   = Item(),
			  const Item& itemthree = Item(),
			  const Item& itemfour  = Item()); 
	void SetItem(const Item& item, const int slot);
	ItemGfxID::Type GetItemGfxID(const int slot) const;
	const Texture& GetItemTex(const int slot) const;
	Item& GetItem(const int slot);
	std::string GetItemName(const int slot) const;
	void EquipWeapon(const int slot);
	void EquipAccessory(const int slot);
	Item* GetEquippedWeapon();
	Item* GetEquippedAccessory();
	bool IsEquipped(const int slot) const;
	bool IsEmpty() const;
	bool HasAccessories() const;

private:
	Item m_items[4];
	int m_equippedWpnSlot;
	int m_equippedAccSlot;
	Item* m_pEquippedWeapon;
	Item* m_pEquippedAccessory;
};

class InventoryRenderer : public GameObject
{
public:
	InventoryRenderer();
	virtual ~InventoryRenderer();
	virtual void DoDraw(DrawMgr& drawMgr);
	void SetInventory(Inventory* pInv);
	void SetInventoryHolder(Actor* pActor);
	void SetRenderMode(const bool vert = false, const bool textmode = false);
	void SetFilter(const InventoryFilter::Type filter);
	void SetChoice(const Direction::Type dir);
	void ShowItemList();
	void ShowItemStats();
	void EquipSelection();

private:
	void InsertItemIntoStream(const int itemnum, std::stringstream& stream);
	int ConvertDirToChoice(const Direction::Type dir);

	Inventory* m_pInventory;
	Actor* m_pActor;
	bool m_vertical;
	bool m_textOnly;
	GameString m_itemText;

	Texture m_iconBracket;
	Texture m_nameBracket;

	int m_filteredInventory[4];

	Direction::Type m_currChoice;

	InventoryFilter::Type m_filter;
};

#endif //_item_h_