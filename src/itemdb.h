#ifndef _itemdb_h_
#define _itemdb_h_

#include <map>
#include <string>
class Item;
class ItemParams;

class ItemDB
{
public:
	static ItemDB& Instance() { static ItemDB instance;  return instance; };

	void AddItem(const ItemParams& itemparams);
	const Item& GetItem(const std::string& itemname) const;
	void RemoveItem(const std::string& itemname);

private:
	ItemDB();
	ItemDB(const ItemDB& rhs);
	ItemDB& operator=(const ItemDB& rhs);

	typedef std::map<std::string, Item> MapNameToItem;

	MapNameToItem m_itemMap;
};

#endif //_itemdb_h_