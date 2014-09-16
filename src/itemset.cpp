#include "itemset.h"
#include "filepathmgr.h"

//////////////////
// ItemGfxID	//
//////////////////

namespace ItemGfxID
{
	const char* Names[] = 
	{
		"Nothing", 
		// Restorative items
		"Medical Herb", "Healing Seed", "Healing Drop", "Antidote", "Angel Wing", "Fairy Powder", 
		"Healing Water", "Fairy Tear", "Healing Rain", 
		// Stat boosting items
		"Power Water", "Protect Milk", "Quick Chicken", "Running Pimento", "Cheerful Bread",
		"Bright Honey", "Brave Apple",
		// One use spell items
		"Shining Ball", "Blizzard", "Holy Thunder",
		// Rings
		"Power Ring", "Protect Ring", "Quick Ring", "Running Ring", "White Ring", "Black Ring",
		"Evil Ring", "Life Ring",
		// Gloves
		"Leather Glove", "Power Glove", "Brass Knuckles", "Iron Knuckles", "Misty Knuckles",
		"Giant Knuckles", "Evil Knuckles",
		// Axes
		"Short Axe", "Hand Axe", "Middle Axe", "Power Axe", "Battle Axe", "Large Axe", "Great Axe",
		"Heat Axe", "Atlas Axe", "Ground Axe", "Rune Axe", "Evil Axe",
		// Projectiles
		"Wooden Arrow", "Iron Arrow", "Steel Arrow", "Robin Arrow", "Assault Shell", "Great Shot",
		"Nazca Cannon", "Buster Shot", "Hyper Cannon", "Grand Cannon", "Evil Shot",
		// Swords
		"Wooden Sword", "Short Sword", "Middle Sword", "Long Sword", "Steel Sword", "Achilles Sword",
		"Broad Sword", "Buster Sword", "Great Sword", "Critical Sword", "Battle Sword", "Force Sword",
		"Counter Sword", "Levanter", "Dark Sword",
		// Spears
		"Wooden Stick", "Short Spear", "Bronze Lance", "Spear", "Steel Lance", "Power Spear",
		"Heavy Lance", "Javelin", "Chrome Lance", "Valkyrie", "Holy Lance", "Mist Javelin",
		"Halberd", "Evil Lance",
		// Staves
		"Wooden Rod", "Short Rod", "Bronze Rod", "Iron Rod", "Power Stick", "Flail", "Guardian Staff",
		"Indra Staff", "Mage Staff", "Wish Staff", "Great Rod", "Supply Staff", "Holy Staff",
		"Freeze Staff", "Goddess Staff", "Mystery Staff", "Demon Rod", "Iron Ball",
		// Daggers
		"Short Knife", "Dagger", "Knife", "Thieves' Dagger",
		// Katanas
		"Katana", "Ninja Katana", "Gisarme", 
		// Special items
		"Taros Sword", "Light Of Hope", "Wooden Panel", "Sky Orb", "Cannon", "Dry Stone", "Dynamite",
		"Arm Of Golem", "Pegasus Wing", "Warrior Pride", "Silver Tank", "Secret Book", "Vigor Ball",
		"Mithril", "Cotton Balloon", "Chirrup Sandles", "Empty"
	};

	CT_ASSERT(sizeof(Names)/sizeof(Names[0]) == NumTypes);
}

//////////////////
// ItemSet		//
//////////////////

ItemSet::ItemSet(const std::string& filename)
{
	LoadItemSet(filename);
}

const Texture& ItemSet::GetItemIcon(ItemGfxID::Type icon) const
{
	return m_itemIcons[icon];
}

void ItemSet::LoadItemSet(const std::string& filename)
{
	Image buffer;
	std::string imagepath = FilePathMgr::Instance().GetImagePath() + filename;
	buffer.LoadBMP(imagepath.c_str());

	for (int i = 0; i < ItemGfxID::NumTypes; i++)
	{
		m_itemIcons[i].LoadSubImage(buffer, (i*16), 0, 16, 24);
	}
}