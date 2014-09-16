#ifndef _commandmgr_h_
#define _commandmgr_h_

#include <queue>
#include "tokenizer.h"
#include "gamecommand.h"
class ActorParams;
class ItemParams;

typedef std::queue<GameCommand*> GameCommandQueue;

class CommandMgr
{
public:
	// returns singleton instance
	static CommandMgr& Instance() { static CommandMgr instance;  return instance; };
	~CommandMgr();

	void SetMode(const CommandMode::Type mode);

	void LoadItemStartupScript();
	void LoadGameStartupScript();
	void LoadMapScript(const std::string& mapname);
	void LoadEventScript(const int eventnum);
	void LoadActorScript(const int actornum);
	void LoadConsoleScript();

	void AddCommandFromToken();
	void AddCommandManually(GameCommand* pGC);
	void GoToNextEnd();
	bool HasActiveCommands() const;
	std::string GetEventScriptName(const unsigned int scriptnum);
	void Update(const float ticks);

	ActorParams* GetActorParams();
	ItemParams* GetItemParams();
	void CreateActor();
	void CreateItem();

private:
	CommandMgr();
	CommandMgr(const CommandMgr& rhs);
	CommandMgr& operator=(const CommandMgr& rhs);

	void LoadScript(const std::string& scriptname);

	GameCommandQueue m_comQueue;
	GameCommand* m_pCurrCommand;

	Tokenizer m_tokenizer;
	Token m_token;

	CommandMode::Type m_mode;

	ActorParams* m_pActorParams;
	ItemParams* m_pItemParams;
};

#endif //_commandmgr_h_