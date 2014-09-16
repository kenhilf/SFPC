#include "commandmgr.h"
#include <sstream>
#include <fstream>
#include "gameobjectmgr.h"
#include "filepathmgr.h"
#include "actorparams.h"
#include "actormgr.h"
#include "itemparams.h"
#include "itemdb.h"
#include "item.h"

CommandMgr::CommandMgr() :
	m_pCurrCommand(NULL),
	m_mode(CommandMode::Normal),
	m_pActorParams(NULL),
	m_pItemParams(NULL)
{

}

CommandMgr::~CommandMgr()
{
	while (!m_comQueue.empty())
	{
		delete m_comQueue.front();
		m_comQueue.pop();
	}

	if (m_pActorParams != NULL)
	{
		delete m_pActorParams;
	}

	if (m_pItemParams != NULL)
	{
		delete m_pItemParams;
	}
}

void CommandMgr::SetMode(const CommandMode::Type mode)
{
	m_mode = mode;

	if (m_mode == CommandMode::ActorSetup)
	{
		if (m_pActorParams != NULL)
		{
			delete m_pActorParams;
		}
		m_pActorParams = new ActorParams;
	}

	if (m_mode == CommandMode::ItemSetup)
	{
		if (m_pItemParams != NULL)
		{
			delete m_pItemParams;
		}
		m_pItemParams = new ItemParams;
	}
/*
	if (mode == CommandMode::Normal)
	{
		// no special actions for Normal mode
	}
*/
/*
	if (mode == CommandMode::MapEditing)
	{
		// no special actions for MapEditing mode
	}
*/
}

void CommandMgr::LoadItemStartupScript()
{
	LoadScript(FilePathMgr::Instance().GetScriptPath() + "items.sfs");
	//@HACK: force startup through
	Update(16.0f);
}

void CommandMgr::LoadGameStartupScript()
{
	LoadScript(FilePathMgr::Instance().GetScriptPath() + "startup.sfs");
	//@HACK: force startup through
	Update(16.0f);
}

void CommandMgr::LoadMapScript(const std::string& mapname)
{
	LoadScript(FilePathMgr::Instance().GetMapScriptPath() + "m000.sfs");
}

void CommandMgr::LoadEventScript(const int eventnum)
{
	std::stringstream filename;
	filename.flush();
	filename << "e";
	if (eventnum < 100)
		filename << "0";
	if (eventnum < 10)
		filename << "0";
	filename << eventnum << ".sfs";

	LoadScript(FilePathMgr::Instance().GetMapScriptPath() + filename.str());
}

void CommandMgr::LoadActorScript(const int actornum)
{
	std::stringstream filename;
	filename.flush();
	filename << "a";
	if (actornum < 100)
		filename << "0";
	if (actornum < 10)
		filename << "0";
	filename << actornum << ".sfs";

	LoadScript(FilePathMgr::Instance().GetMapScriptPath() + filename.str());
}

void CommandMgr::LoadConsoleScript()
{
	LoadScript(FilePathMgr::Instance().GetScriptPath() + "console.sfs");
}

void CommandMgr::AddCommandFromToken()
{
 	GameCommand* pGC = NULL;

	// Mode independent commands
	if (m_token.AsString() == "SetCommandMode")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		pGC = new GCSetCommandMode(m_token.AsString());
		m_comQueue.push(pGC);
		return;
	}

	if (m_token.AsString() == "ResetCommandMode")
	{
		pGC = new GCResetCommandMode();
		m_comQueue.push(pGC);
		return;
	}

	// ActorSetup Mode only commands
	if (m_token.AsString() == "BeginNewActor")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		std::string name = m_token.AsString();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		pGC = new GCBeginNewActor(name, m_token.AsString());
		m_comQueue.push(pGC);
		return;
	}

	if (m_token.AsString() == "EndNewActor")
	{
		pGC = new GCEndNewActor();
		m_comQueue.push(pGC);
		return;			
	}

	if (m_token.AsString() == "SetActorClass")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		pGC = new GCSetActorClass(m_token.AsString());
		m_comQueue.push(pGC);
		return;
	}

	if (m_token.AsString() == "SetActorMobility")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		pGC = new GCSetActorMobility(m_token.AsString());
		m_comQueue.push(pGC);
		return;
	}

	if (m_token.AsString() == "SetActorStats")
	{
		int level, hp, mp, exp, atk, def, agi;
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		level = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		hp = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		mp = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		exp = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		atk = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		def = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		agi = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		pGC = new GCSetActorStats(level, hp, mp, exp, atk, def, agi, m_token.AsInt());
		m_comQueue.push(pGC);
		return;			
	}

	if (m_token.AsString() == "SetActorTileset")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		pGC = new GCSetActorTileset(m_token.AsString());
		m_comQueue.push(pGC);
		return;
	}

	if (m_token.AsString() == "SetActorFaceset")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		pGC = new GCSetActorFaceset(m_token.AsString());
		m_comQueue.push(pGC);
		return;
	}

	if (m_token.AsString() == "SetActorBattleset")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		std::string bsname = m_token.AsString();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		pGC = new GCSetActorBattleset(bsname, m_token.AsInt());
		m_comQueue.push(pGC);
		return;
	}

	if (m_token.AsString() == "SetActorPosition")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		int x = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		int y = m_token.AsInt();
		pGC = new GCSetActorPosition(x, y);
		m_comQueue.push(pGC);
		return;			
	}

	if (m_token.AsString() == "SetActorFacing")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		pGC = new GCSetActorFacing(m_token.AsString());
		m_comQueue.push(pGC);
		return;
	}

	if (m_token.AsString() == "SetActorSpell")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		pGC = new GCSetActorSpell(m_token.AsString());
		m_comQueue.push(pGC);
		return;
	}

	if (m_token.AsString() == "SetActorItem")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		pGC = new GCSetActorItem(m_token.AsString());
		m_comQueue.push(pGC);
		return;
	}

	if (m_token.AsString() == "SetActorItemEquipped")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		pGC = new GCSetActorItemEquipped(m_token.AsString());
		m_comQueue.push(pGC);
		return;
	}

	if (m_token.AsString() == "PlaceAlly")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		int x = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		int y = m_token.AsInt();
		pGC = new GCPlaceAlly(x, y);
		m_comQueue.push(pGC);
		return;			
	}

	if (m_token.AsString() == "AddPathStep")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		pGC = new GCAddPathStep(m_token.AsString());
		m_comQueue.push(pGC);
		return;
	}

	// ItemSetup Mode only commands
	if (m_token.AsString() == "BeginNewItem")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		pGC = new GCBeginNewItem(m_token.AsString());
		m_comQueue.push(pGC);
		return;
	}

	if (m_token.AsString() == "EndNewItem")
	{
		pGC = new GCEndNewItem();
		m_comQueue.push(pGC);
		return;			
	}

	if (m_token.AsString() == "SetItemClass")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		pGC = new GCSetItemClass(m_token.AsString());
		m_comQueue.push(pGC);
		return;
	}

	if (m_token.AsString() == "SetItemIcon")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		pGC = new GCSetItemIcon(m_token.AsString());
		m_comQueue.push(pGC);
		return;
	}

	if (m_token.AsString() == "SetCutsceneGfxFilename")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		pGC = new GCSetCutsceneGfxFilename(m_token.AsString());
		m_comQueue.push(pGC);
		return;
	}

	if (m_token.AsString() == "SetItemStats")
	{
		int hp, mp, atk, def, agi;
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		hp = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		mp = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		atk = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		def = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		agi = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		pGC = new GCSetItemStats(hp, mp, atk, def, agi, m_token.AsInt());
		m_comQueue.push(pGC);
		return;			
	}

	if (m_token.AsString() == "SetItemGoldValue")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		pGC = new GCSetItemGoldValue(m_token.AsInt());
		m_comQueue.push(pGC);
		return;			
	}

	if (m_token.AsString() == "AddItemRange")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		pGC = new GCAddItemRange(m_token.AsInt());
		m_comQueue.push(pGC);
		return;			
	}

	if (m_token.AsString() == "AddItemEquipable")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		pGC = new GCAddItemEquipable(m_token.AsString());
		m_comQueue.push(pGC);
		return;
	}

	// Normal Mode only commands
	if (m_token.AsString() == "DialogueWindow")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		pGC = new GCDialogueWindow(m_token.AsString());
		m_comQueue.push(pGC);
		return;
	}

	if (m_token.AsString() == "FaceWindow")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		std::string text = m_token.AsString();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		//@HACK: need to rethink how to reference facewindows
		// maybe make a FaceSetMgr, map names to facesets?
		ActorList& actorlist = GameObjectMgr::Instance().GetActorList();
		pGC = new GCFaceWindow(text, actorlist[m_token.AsInt()]);
		m_comQueue.push(pGC);
		return;
	}

	if (m_token.AsString() == "FlagNeeded")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		pGC = new GCFlagNeeded(m_token.AsString());
		m_comQueue.push(pGC);
		return;
	}

	if (m_token.AsString() == "NoFlag")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		pGC = new GCNoFlag(m_token.AsString());
		m_comQueue.push(pGC);
		return;
	}

	if (m_token.AsString() == "SetFlag")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		pGC = new GCSetFlag(m_token.AsString());
		m_comQueue.push(pGC);
		return;
	}

	if (m_token.AsString() == "RemoveFlag")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		pGC = new GCRemoveFlag(m_token.AsString());
		m_comQueue.push(pGC);
		return;
	}

	if (m_token.AsString() == "End")
	{
		pGC = new GCEnd();
		m_comQueue.push(pGC);
		return;			
	}

	if (m_token.AsString() == "CheckDirection")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		pGC = new GCCheckDirection(Direction::Convert::NameToEnum(m_token.AsString().c_str()));
		m_comQueue.push(pGC);
		return;
	}

	if (m_token.AsString() == "ToggleSolid")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		int x = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		int y = m_token.AsInt();
		pGC = new GCToggleSolid(x, y);
		m_comQueue.push(pGC);
		return;			
	}

	if (m_token.AsString() == "SetBackgroundTile")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		int tilenum = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		int x = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		int y = m_token.AsInt();
		pGC = new GCSetBackgroundTile(tilenum, x, y);
		m_comQueue.push(pGC);
		return;			
	}

	if (m_token.AsString() == "SetOverlayTile")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		int tilenum = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		int x = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		int y = m_token.AsInt();
		pGC = new GCSetOverlayTile(tilenum, x, y);
		m_comQueue.push(pGC);
		return;			
	}

	if (m_token.AsString() == "SetForegroundTile")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		int tilenum = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		int x = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		int y = m_token.AsInt();
		pGC = new GCSetForegroundTile(tilenum, x, y);
		m_comQueue.push(pGC);
		return;			
	}
	
	if (m_token.AsString() == "AddItem")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		std::string item = m_token.AsString().c_str();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		pGC = new GCAddItem(item, m_token.AsString());
		m_comQueue.push(pGC);
		return;			
	}

	if (m_token.AsString() == "SetEvent")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		int eventnum = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		int x = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		int y = m_token.AsInt();
		pGC = new GCSetEvent(eventnum, x, y);
		m_comQueue.push(pGC);
		return;			
	}

	if (m_token.AsString() == "FadeIn")
	{
		pGC = new GCFadeIn();
		m_comQueue.push(pGC);
		return;			
	}

	if (m_token.AsString() == "FadeOut")
	{
		pGC = new GCFadeOut();
		m_comQueue.push(pGC);
		return;			
	}

	if (m_token.AsString() == "Warp")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		int x = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		int y = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		std::string mapname = m_token.AsString();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		Direction::Type dir = Direction::Convert::NameToEnum(m_token.AsString().c_str());
		pGC = new GCWarp(x, y, mapname, dir);
		m_comQueue.push(pGC);
		return;			
	}

	if (m_token.AsString() == "Wait")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		pGC = new GCWait(m_token.AsInt());
		m_comQueue.push(pGC);
		return;			
	}

	if (m_token.AsString() == "PlayMusic")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		pGC = new GCPlayMusic(m_token.AsString());
		m_comQueue.push(pGC);
		return;
	}

	if (m_token.AsString() == "PlaySound")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		pGC = new GCPlaySound(m_token.AsString());
		m_comQueue.push(pGC);
		return;
	}

	if (m_token.AsString() == "StartBattle")
	{
		pGC = new GCStartBattle();
		m_comQueue.push(pGC);
		return;			
	}

	if (m_token.AsString() == "EndBattle")
	{
		pGC = new GCEndBattle();
		m_comQueue.push(pGC);
		return;			
	}

	if (m_token.AsString() == "CreateNewMap")
	{
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::String));
		std::string mapname = m_token.AsString();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		int w = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		int h = m_token.AsInt();
		pGC = new GCCreateNewMap(mapname, w, h);
		m_comQueue.push(pGC);
		return;			
	}

	if (m_token.AsString() == "TileFill")
	{
		int tilenum, layer, x1, y1, x2;
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		tilenum = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		layer = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		x1 = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		y1 = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		x2 = m_token.AsInt();
		VERIFY((m_tokenizer.GetNextToken(m_token)) && (m_token.GetType() == TokenID::Integer));
		pGC = new GCTileFill(tilenum, layer, x1, y1, x2, m_token.AsInt());
		m_comQueue.push(pGC);
		return;			
	}

	assert(false && "Unknown token in script, cannot load command!");
}

void CommandMgr::AddCommandManually(GameCommand* pGC)
{
	m_comQueue.push(pGC);
}

void CommandMgr::GoToNextEnd()
{
	GameCommand* pSkipCommand = NULL;

	while ((m_comQueue.front()->GetID() != CommandID::End) && (!m_comQueue.empty()))
	{
		pSkipCommand = m_comQueue.front();
		m_comQueue.pop();
		delete pSkipCommand;
		pSkipCommand = NULL;
	}
}

bool CommandMgr::HasActiveCommands() const
{
	if ((m_comQueue.size() > 0) || (m_pCurrCommand != NULL))
		return true;
	else
		return false;
}

std::string CommandMgr::GetEventScriptName(const unsigned int scriptnum)
{
	if (scriptnum < 1)
		return "No Event";

	std::stringstream filename;
	filename << "e";
	if (scriptnum < 100)
		filename << "0";
	if (scriptnum < 10)
		filename << "0";
	filename << scriptnum << ".sfs";

	const std::string scriptFilename = FilePathMgr::Instance().GetMapScriptPath() + filename.str();

	std::ifstream infile;
	infile.open(scriptFilename.c_str());
	if (!(infile.is_open()))
		assert(false && "Failed to open script file " && scriptFilename.c_str());

	std::string buffer;
	infile >> buffer;
	assert(buffer == "!SFPC_Script");
	getline(infile, buffer);
	m_tokenizer.SetString(buffer);
	VERIFY(m_tokenizer.GetNextToken(m_token));
	assert(m_token.GetType() == TokenID::String);
	infile.close();

	return m_token.AsString();
}

void CommandMgr::Update(const float ticks)
{
	if ((m_pCurrCommand == NULL) && (m_comQueue.size() > 0))
	{
		m_pCurrCommand = m_comQueue.front();
		m_comQueue.pop();
	}

	while (m_pCurrCommand)
	{
		if (m_pCurrCommand->Update(ticks))
		{
			delete m_pCurrCommand;
			m_pCurrCommand = NULL;

			if (m_comQueue.size() > 0)
			{
				m_pCurrCommand = m_comQueue.front();
				m_comQueue.pop();
			}
		}
		else
			break;
	}
}

void CommandMgr::LoadScript(const std::string& scriptname)
{
	std::ifstream infile;
	infile.open(scriptname.c_str());
	if (!(infile.is_open()))
		assert(false && "Failed to open script file " && scriptname.c_str());

	std::string buffer;
	infile >> buffer;
	assert(buffer == "!SFPC_Script");
	getline(infile, buffer);
	m_tokenizer.SetString(buffer);
	VERIFY(m_tokenizer.GetNextToken(m_token));
	assert(m_token.GetType() == TokenID::String); 

	while (!infile.eof())
	{
		getline(infile, buffer);
		m_tokenizer.SetString(buffer);
		if (m_tokenizer.GetNextToken(m_token))
		{
			if (m_token.GetType() == TokenID::Identifier)
			{
				AddCommandFromToken();
			}
			else
			{
				assert(false && "Bad command in script file " && scriptname.c_str());
			}
		}
	}

	infile.close();
}

ActorParams* CommandMgr::GetActorParams()
{
	assert((m_pActorParams != NULL) && (m_mode == CommandMode::ActorSetup));
	return m_pActorParams;
}

ItemParams* CommandMgr::GetItemParams()
{
	assert((m_pItemParams != NULL) && (m_mode == CommandMode::ItemSetup));
	return m_pItemParams;
}

void CommandMgr::CreateActor()
{
	assert((m_pActorParams != NULL) && (m_mode == CommandMode::ActorSetup));

	ActorTeam::Type team = ActorTeam::Convert::NameToEnum(m_pActorParams->m_team.c_str());
	switch (team)
	{
		case ActorTeam::Player:
			ActorMgr::Instance().AddPlayer(*m_pActorParams);
			break;
		case ActorTeam::NPC:
			ActorMgr::Instance().AddNPC(*m_pActorParams);
			break;
		case ActorTeam::Enemy:
			ActorMgr::Instance().AddEnemy(*m_pActorParams);
			break;
		default:
			assert(false && "Invalid team specified for Actor creation!");
			break;
	}

	delete m_pActorParams;
	m_pActorParams = new ActorParams;
}

void CommandMgr::CreateItem()
{
	assert((m_pItemParams != NULL) && (m_mode == CommandMode::ItemSetup));

	ItemDB::Instance().AddItem(*m_pItemParams);

	delete m_pItemParams;
	m_pItemParams = new ItemParams;
}