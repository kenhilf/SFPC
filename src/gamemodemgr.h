#ifndef _gamemodemgr_h_
#define _gamemodemgr_h_

namespace GameMode
{
	enum Type { Field, Battle, EditMap, NumTypes };
}

// using Tony's singleton replacement pattern for hiding private vars
namespace GameModeMgr
{
	void Init();
	GameMode::Type GetMode();
	void SetMode(const GameMode::Type mode);
};

#endif //_gamemodemgr_h_