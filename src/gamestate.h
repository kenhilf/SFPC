#ifndef _gamestate_h_
#define _gamestate_h_

#include <map>
#include <string>

class GameState
{
public:
	static GameState& Instance() { static GameState instance;  return instance; };

	void SetFlag(const std::string& flag, const int val = 1);
	int GetFlag(const std::string& flag);
	void RemoveFlag(const std::string& flag);

private:
	GameState();
	GameState(const GameState& rhs);
	GameState& operator=(const GameState& rhs);

	typedef std::map<std::string, int> MapFlagToInt;

	MapFlagToInt m_flagMap;
};

#endif //_gamestate_h_