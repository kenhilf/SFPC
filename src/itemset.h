#ifndef _itemset_h_
#define _itemset_h_

#include "core.h"
#include "texture.h"
#include <string>

namespace ItemGfxID
{
	enum Type 
	{			
		Nothing, 
		// Restorative items
		MedicalHerb, HealingSeed, HealingDrop, Antidote, AngelWing, FairyPowder, 
		HealingWater, FairyTear, HealingRain, 
		// Stat boosting items
		PowerWater, ProtectMilk, QuickChicken, RunningPimento, CheerfulBread,
		BrightHoney, BraveApple,
		// One use spell items
		ShiningBall, Blizzard, HolyThunder,
		// Rings
		PowerRing, ProtectRing, QuickRing, RunningRing, WhiteRing, BlackRing,
		EvilRing, LifeRing,
		// Gloves
		LeatherGlove, PowerGlove, BrassKnuckles, IronKnuckles, MistyKnuckles,
		GiantKnuckles, EvilKnuckles,
		// Axes
		ShortAxe, HandAxe, MiddleAxe, PowerAxe, BattleAxe, LargeAxe, GreatAxe,
		HeatAxe, AtlasAxe, GroundAxe, RuneAxe, EvilAxe,
		// Projectiles
		WoodenArrow, IronArrow, SteelArrow, RobinArrow, AssaultShell, GreatShot,
		NazcaCannon, BusterShot, HyperCannon, GrandCannon, EvilShot,
		// Swords
		WoodenSword, ShortSword, MiddleSword, LongSword, SteelSword, AchillesSword,
		BroadSword, BusterSword, GreatSword, CriticalSword, BattleSword, ForceSword,
		CounterSword, Levanter, DarkSword,
		// Spears
		WoodenStick, ShortSpear, BronzeLance, Spear, SteelLance, PowerSpear,
		HeavyLance, Javelin, ChromeLance, Valkyrie, HolyLance, MistJavelin,
		Halberd, EvilLance,
		// Staves
		WoodenRod, ShortRod, BronzeRod, IronRod, PowerStick, Flail, GuardianStaff,
		IndraStaff, MageStaff, WishStaff, GreatRod, SupplyStaff, HolyStaff,
		FreezeStaff, GoddessStaff, MysteryStaff, DemonRod, IronBall,
		// Daggers
		ShortKnife, Dagger, Knife, ThievesDagger,
		// Katanas
		Katana, NinjaKatana, Gisarme, 
		// Special items
		TarosSword, LightOfHope, WoodenPanel, SkyOrb, Cannon, DryStone, Dynamite,
		ArmOfGolem, PegasusWing, WarriorPride, SilverTank, SecretBook, VigorBall,
		Mithril, CottonBalloon, ChirrupSandles, Empty,
		NumTypes
	};

	extern const char* Names[];

	typedef EnumNameConverterT<Type, NumTypes, Names> Convert;
}

class ItemSet
{
public:
	ItemSet(const std::string& filename);
	const Texture& GetItemIcon(ItemGfxID::Type icon) const;

private:
	void LoadItemSet(const std::string& filename);

	Texture m_itemIcons[ItemGfxID::NumTypes];
};


#endif //_itemset_h_