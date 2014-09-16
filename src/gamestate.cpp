#include "gamestate.h"

GameState::GameState()
{

}

void GameState::SetFlag(const std::string& flag, const int val)
{
	m_flagMap[flag] = val;
}

int GameState::GetFlag(const std::string& flag)
{
	MapFlagToInt::iterator iter = m_flagMap.find(flag);
	if (iter == m_flagMap.end())
		return 0;
	else
		return iter->second;
}

void GameState::RemoveFlag(const std::string& flag)
{
	m_flagMap.erase(flag);
}