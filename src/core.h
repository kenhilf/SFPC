#ifndef _core_h_
#define _core_h_

#include <assert.h>

const int GAME_SCREEN_WIDTH = 320;
const int GAME_SCREEN_HEIGHT = 240;
const int ACTUAL_SCREEN_WIDTH = 1280;
const int ACTUAL_SCREEN_HEIGHT = 960;
const int TILE_WIDTH = 24;
const int TILE_HEIGHT = 24;
const int GAME_SCREEN_WIDTH_IN_TILES = 14;
const int GAME_SCREEN_HEIGHT_IN_TILES = 10;
const int TILEMAP_WIDTH = 100;
const int TILEMAP_HEIGHT = 100;

const char COMMENT_CHAR = '#';
const char EOL_CHAR = '\n';
const char SCRIPT_EOL_CHAR = '$';
const char PAUSE_CHAR = '^';
const char SCROLL_CHAR = '@';
const char TINT_CHAR = '*';

#define ASSERT assert
#define CT_ASSERT(cond) int ct_assert[cond? 1 : 0]

#ifndef NDEBUG
#define VERIFY(exp) do { bool result = (exp); assert(result); } while (false)
#else
#define VERIFY(exp) (exp)
#endif

#ifndef NULL
#define NULL 0
#endif

template <typename T>
T Pow3(T aValue)
{
    return aValue * aValue * aValue;
}

template <typename EnumType, int NumTypes, const char* Names[]>
class EnumNameConverterT
{
public:
	static EnumType NameToEnum(const char* name)
	{
		for (int i = 0; i < NumTypes; ++i)
		{
			if (strcmp(name, Names[i]) == 0)
				return static_cast<EnumType>(i);
		}

		return static_cast<EnumType>(NumTypes);
	}

	static const char* EnumToName(EnumType e)
	{
		ASSERT(e >= 0 && e < NumTypes);
		return Names[e];
	}
};

namespace Direction 
{
	enum Type { Down, Up, Left, Right, None, NumTypes };

	extern const char* Names[];

	typedef EnumNameConverterT<Type, NumTypes, Names> Convert;

	Type Opposite(Type dir);
}

namespace ActorTeam
{
	enum Type { Player, NPC, Enemy, Editor, NumTypes };

	extern const char* Names[];

	typedef EnumNameConverterT<Type, NumTypes, Names> Convert;

	Type Opponent(Type opp);
}

namespace CompassDir
{
	enum Type { N, NE, E, SE, S, SW, W, NW, NumTypes };
}

namespace ActionName
{
	enum Type { Yes, No, Attack, Magic, Item, Stay, Search, Member, Talk, Use, Give, 
				Equip, Drop, Map, Speed, Quit, Raise, Cure, Promote, Save, Buy, Sell, 
				Repair, Deals, Join, Depot, Purge, Look, Deposit, Derive, NumTypes };
}

namespace CharacterClass
{
	enum Type { NONE, SDMN, PRST, KNTE, WARR, MAGE, THIF, TORT, PHNK, RNGR, WFMN, BDMN, 
				BRGN, ACHR, PLDN, SORC, BDBT, VICR, PGNT, GLDT, MMNK, RBT, GLM, WIZ, RDBN,
				HERO, BRN, NINJ, MNST, PHNX, BWNT, WFBR, SNIP, ALL, NumTypes };

	extern const char* Names[];

	typedef EnumNameConverterT<Type, NumTypes, Names> Convert;
}

namespace Terrain
{
	enum Type { Smooth, Rough, Forest, Hills, Water, Mountains, NumTypes };
}

namespace DamageElement
{
	enum Type { Physical, Fire, Cold, Elec, Wind, Almighty, NumTypes };

	extern const char* Names[];

	typedef EnumNameConverterT<Type, NumTypes, Names> Convert;
}

struct Pixel24
{
	unsigned char r, g, b;
};

struct Pixel32
{
	Pixel32() : r(0), g(0), b(0), a(0) { }

	unsigned char r, g, b, a;
};

float EaseInOut(float aUnitValue);

#endif //_core_h_