#include "gamemodemgr.h"
#include <assert.h>

namespace GameModeMgr
{
static GameMode::Type g_gameMode;

void Init()
{
	g_gameMode = GameMode::Field;
}

GameMode::Type GetMode()
{
	return g_gameMode;
}

void SetMode(const GameMode::Type mode)
{
	assert(mode < GameMode::NumTypes);
	g_gameMode = mode;
}

}
