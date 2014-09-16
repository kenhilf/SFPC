#include "itemdb.h"
#include "item.h"
#include "itemparams.h"

ItemDB::ItemDB()
{

}

void ItemDB::AddItem(const ItemParams& itemparams)
{
	MapNameToItem::const_iterator iter = m_itemMap.find(itemparams.m_name);
	if (!(iter == m_itemMap.end()))
	{
		assert(false && "Error adding item, item already exists!");
	}

	m_itemMap[itemparams.m_name] = Item(itemparams);
}

const Item& ItemDB::GetItem(const std::string& itemname) const
{
	MapNameToItem::const_iterator iter = m_itemMap.find(itemname);
	if (iter == m_itemMap.end())
	{
		assert(false && "Error looking up item!");
	}
	
	return iter->second;
}

void ItemDB::RemoveItem(const std::string& itemname)
{
	m_itemMap.erase(itemname);
}
