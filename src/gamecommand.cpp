#include "gamecommand.h"
#include <assert.h>
#include <sstream>
#include "windowmgr.h"
#include "actor.h"
#include "tilemap.h"
#include "commandmgr.h"
#include "filepathmgr.h"
#include "audiomgr.h"
#include "actormgr.h"
#include "itemdb.h"
#include "statblock.h"
#include "gameobjectmgr.h"
#include "gamestate.h"
#include "gameengine.h"
#include "groundbattle.h"
#include "outro.h"

//////////////////////
// CommandMode		//
//////////////////////

namespace CommandMode
{
	const char* Names[] = { "Normal", "ActorSetup", "ItemSetup", "MapEditing" };

	CT_ASSERT(sizeof(Names)/sizeof(Names[0]) == NumTypes);
}

//////////////////////
// GameCommand		//
//////////////////////

GameCommand::GameCommand(const CommandID::Type id) :
	m_commandID(id)
{

}

GameCommand::~GameCommand()
{

}

CommandID::Type GameCommand::GetID()
{
	return m_commandID;
}

bool GameCommand::Update(const float ticks)
{
	return true;
}

/////////////////////////////////////
	// Mode Shifting Commands //     
/////////////////////////////////////

//////////////////////
// GCSetCommandMode	//
//////////////////////

GCSetCommandMode::GCSetCommandMode(const std::string& mode) :
	GameCommand(CommandID::SetCommandMode),
	m_comMode(mode)
{

}

GCSetCommandMode::~GCSetCommandMode()
{

}

bool GCSetCommandMode::Update(const float ticks)
{
	CommandMgr::Instance().SetMode(CommandMode::Convert::NameToEnum(m_comMode.c_str()));
	return true;
}

////////////////////////
// GCResetCommandMode //
////////////////////////

GCResetCommandMode::GCResetCommandMode() :
	GameCommand(CommandID::ResetCommandMode)
{

}

GCResetCommandMode::~GCResetCommandMode()
{

}

bool GCResetCommandMode::Update(const float ticks)
{
	CommandMgr::Instance().SetMode(CommandMode::Normal);
	return true;
}

//////////////////////////////////////
	// ActorSetup Mode Commands //     
//////////////////////////////////////

//////////////////////
// GCBeginNewActor	//
//////////////////////

GCBeginNewActor::GCBeginNewActor(const std::string& name, const std::string& team) :
	GameCommand(CommandID::BeginNewActor),
	m_name(name),
	m_team(team)
{

}

GCBeginNewActor::~GCBeginNewActor()
{

}

bool GCBeginNewActor::Update(const float ticks)
{
	ActorParams* pAP = CommandMgr::Instance().GetActorParams();
	pAP->m_name = m_name;
	pAP->m_team = m_team;
	return true;
}

//////////////////////
// GCEndNewActor	//
//////////////////////

GCEndNewActor::GCEndNewActor() :
	GameCommand(CommandID::EndNewActor)
{

}

GCEndNewActor::~GCEndNewActor()
{

}

bool GCEndNewActor::Update(const float ticks)
{
	CommandMgr::Instance().CreateActor();
	return true;
}

//////////////////////
// GCSetActorClass	//
//////////////////////

GCSetActorClass::GCSetActorClass(const std::string& actorclass) :
	GameCommand(CommandID::SetActorClass),
	m_class(actorclass)
{

}

GCSetActorClass::~GCSetActorClass()
{

}

bool GCSetActorClass::Update(const float ticks)
{
	CommandMgr::Instance().GetActorParams()->m_charclass = m_class;
	return true;
}

////////////////////////
// GCSetActorMobility //
////////////////////////

GCSetActorMobility::GCSetActorMobility(const std::string& mobility) :
	GameCommand(CommandID::SetActorMobility),
	m_mobility(mobility)
{

}

GCSetActorMobility::~GCSetActorMobility()
{

}

bool GCSetActorMobility::Update(const float ticks)
{
	CommandMgr::Instance().GetActorParams()->m_movetype = m_mobility;
	return true;
}

/////////////////////
// GCSetActorStats //
/////////////////////

GCSetActorStats::GCSetActorStats(const int level, const int hp, const int mp, const int exp, 
								 const int atk, const int def, const int agi, const int mov) :
	GameCommand(CommandID::SetActorStats),
	m_level(level), m_hp(hp), m_mp(mp), m_exp(exp), 
	m_atk(atk), m_def(def), m_agi(agi), m_mov(mov)
{

}

GCSetActorStats::~GCSetActorStats()
{

}

bool GCSetActorStats::Update(const float ticks)
{
	ActorParams* pAP = CommandMgr::Instance().GetActorParams();
	pAP->m_level = m_level;
	pAP->m_hp = m_hp;
	pAP->m_mp = m_mp;
	pAP->m_exp = m_exp;
	pAP->m_atk = m_atk;
	pAP->m_def = m_def;
	pAP->m_agi = m_agi;
	pAP->m_mov = m_mov;
	return true;
}

///////////////////////
// GCSetActorTileset //
///////////////////////

GCSetActorTileset::GCSetActorTileset(const std::string& tsname) :
	GameCommand(CommandID::SetActorTileset),
	m_tsname(tsname)
{

}

GCSetActorTileset::~GCSetActorTileset()
{

}

bool GCSetActorTileset::Update(const float ticks)
{
	CommandMgr::Instance().GetActorParams()->m_tilesetName = m_tsname;
	return true;
}

///////////////////////
// GCSetActorFaceset //
///////////////////////

GCSetActorFaceset::GCSetActorFaceset(const std::string& fsname) :
	GameCommand(CommandID::SetActorFaceset),
	m_fsname(fsname)
{

}

GCSetActorFaceset::~GCSetActorFaceset()
{

}

bool GCSetActorFaceset::Update(const float ticks)
{
	CommandMgr::Instance().GetActorParams()->m_facesetName = m_fsname;
	return true;
}

/////////////////////////
// GCSetActorBattleset //
/////////////////////////

GCSetActorBattleset::GCSetActorBattleset(const std::string& bsname, const int numframes) :
	GameCommand(CommandID::SetActorFaceset),
	m_bsname(bsname),
	m_numFrames(numframes)
{

}

GCSetActorBattleset::~GCSetActorBattleset()
{

}

bool GCSetActorBattleset::Update(const float ticks)
{
	ActorParams* pAP = CommandMgr::Instance().GetActorParams();
	pAP->m_battlesetName = m_bsname;
	pAP->m_numBattleFrames = m_numFrames;
	return true;
}

////////////////////////
// GCSetActorPosition //
////////////////////////

GCSetActorPosition::GCSetActorPosition(const int x, const int y) :
	GameCommand(CommandID::SetActorPosition),
	m_x(x), m_y(y)
{

}

GCSetActorPosition::~GCSetActorPosition()
{

}

bool GCSetActorPosition::Update(const float ticks)
{
	ActorParams* pAP = CommandMgr::Instance().GetActorParams();
	pAP->m_xpos = m_x;
	pAP->m_ypos = m_y;
	return true;
}

//////////////////////
// GCSetActorFacing //
//////////////////////

GCSetActorFacing::GCSetActorFacing(const std::string& facing) :
	GameCommand(CommandID::SetActorFacing),
	m_facing(facing)
{

}

GCSetActorFacing::~GCSetActorFacing()
{

}

bool GCSetActorFacing::Update(const float ticks)
{
	CommandMgr::Instance().GetActorParams()->m_facing = m_facing;
	return true;
}

/////////////////////
// GCSetActorSpell //
/////////////////////

GCSetActorSpell::GCSetActorSpell(const std::string& spell) :
	GameCommand(CommandID::SetActorSpell),
	m_spell(spell)
{

}

GCSetActorSpell::~GCSetActorSpell()
{

}

bool GCSetActorSpell::Update(const float ticks)
{
	CommandMgr::Instance().GetActorParams()->AddSpell(m_spell);
	return true;
}

////////////////////
// GCSetActorItem //
////////////////////

GCSetActorItem::GCSetActorItem(const std::string& item) :
	GameCommand(CommandID::SetActorItem),
	m_item(item)
{

}

GCSetActorItem::~GCSetActorItem()
{

}

bool GCSetActorItem::Update(const float ticks)
{
	CommandMgr::Instance().GetActorParams()->AddItem(m_item);
	return true;
}

////////////////////////////
// GCSetActorItemEquipped //
////////////////////////////

GCSetActorItemEquipped::GCSetActorItemEquipped(const std::string& item) :
	GameCommand(CommandID::SetActorItemEquipped),
	m_item(item)
{

}

GCSetActorItemEquipped::~GCSetActorItemEquipped()
{

}

bool GCSetActorItemEquipped::Update(const float ticks)
{
	int slot = CommandMgr::Instance().GetActorParams()->m_numItems;
	CommandMgr::Instance().GetActorParams()->AddItem(m_item);
	CommandMgr::Instance().GetActorParams()->m_bEquipped[slot] = true;
	return true;
}

//////////////////
// GCPlaceAlly	//
//////////////////

GCPlaceAlly::GCPlaceAlly(const int x, const int y) :
	GameCommand(CommandID::PlaceAlly),
	m_x(x),
	m_y(y)
{

}

GCPlaceAlly::~GCPlaceAlly()
{

}

bool GCPlaceAlly::Update(const float ticks)
{
	GroundBattle::Instance().PlaceAlly(Position(m_x, m_y));
	return true;
}

///////////////////
// GCAddPathStep //
///////////////////

GCAddPathStep::GCAddPathStep(const std::string& dir) :
	GameCommand(CommandID::AddPathStep),
	m_direction(dir)
{

}

GCAddPathStep::~GCAddPathStep()
{

}

bool GCAddPathStep::Update(const float ticks)
{
	CommandMgr::Instance().GetActorParams()->m_steps.push_back(Direction::Convert::NameToEnum(m_direction.c_str()));
	return true;
}


//////////////////////////////////////
	// ItemSetup Mode Commands //     
//////////////////////////////////////

//////////////////////
// GCBeginNewItem	//
//////////////////////

GCBeginNewItem::GCBeginNewItem(const std::string& name) :
	GameCommand(CommandID::BeginNewItem),
	m_name(name)
{

}

GCBeginNewItem::~GCBeginNewItem()
{

}

bool GCBeginNewItem::Update(const float ticks)
{
	CommandMgr::Instance().GetItemParams()->m_name = m_name;
	return true;
}

//////////////////////
// GCEndNewItem		//
//////////////////////

GCEndNewItem::GCEndNewItem() :
	GameCommand(CommandID::EndNewItem)
{

}

GCEndNewItem::~GCEndNewItem()
{

}

bool GCEndNewItem::Update(const float ticks)
{
	CommandMgr::Instance().CreateItem();
	return true;
}

//////////////////////
// GCSetItemClass	//
//////////////////////

GCSetItemClass::GCSetItemClass(const std::string& itemclass) :
	GameCommand(CommandID::SetItemClass),
	m_itemClass(itemclass)
{

}

GCSetItemClass::~GCSetItemClass()
{

}

bool GCSetItemClass::Update(const float ticks)
{
	CommandMgr::Instance().GetItemParams()->m_itemClass = 
		ItemClass::Convert::NameToEnum(m_itemClass.c_str());
	return true;
}

//////////////////////
// GCSetItemIcon	//
//////////////////////

GCSetItemIcon::GCSetItemIcon(const std::string& icon) :
	GameCommand(CommandID::SetItemIcon),
	m_itemIcon(icon)
{

}

GCSetItemIcon::~GCSetItemIcon()
{

}

bool GCSetItemIcon::Update(const float ticks)
{
	CommandMgr::Instance().GetItemParams()->m_itemGfxID = 
		ItemGfxID::Convert::NameToEnum(m_itemIcon.c_str());
	return true;
}

//////////////////////////////
// GCSetCutsceneGfxFilename	//
//////////////////////////////

GCSetCutsceneGfxFilename::GCSetCutsceneGfxFilename(const std::string& filename) :
	GameCommand(CommandID::SetCutsceneGfxFilename),
	m_filename(filename)
{

}

GCSetCutsceneGfxFilename::~GCSetCutsceneGfxFilename()
{

}

bool GCSetCutsceneGfxFilename::Update(const float ticks)
{
	CommandMgr::Instance().GetItemParams()->m_cutsceneGfxFilename = m_filename;
	return true;
}

/////////////////////
// GCSetItemStats //
/////////////////////

GCSetItemStats::GCSetItemStats(const int hp, const int mp, const int atk, 
							   const int def, const int agi, const int mov) :
	GameCommand(CommandID::SetItemStats),
	m_hp(hp), m_mp(mp), m_atk(atk), 
	m_def(def), m_agi(agi), m_mov(mov)
{

}

GCSetItemStats::~GCSetItemStats()
{

}

bool GCSetItemStats::Update(const float ticks)
{
	ItemParams* pIP = CommandMgr::Instance().GetItemParams();
	pIP->m_HPMod = m_hp;
	pIP->m_MPMod = m_mp;
	pIP->m_atkMod = m_atk;
	pIP->m_defMod = m_def;
	pIP->m_agiMod = m_agi;
	pIP->m_movMod = m_mov;
	return true;
}

//////////////////////////
// GCSetItemGoldValue	//
//////////////////////////

GCSetItemGoldValue::GCSetItemGoldValue(const int value) :
	GameCommand(CommandID::SetItemGoldValue),
	m_value(value)
{

}

GCSetItemGoldValue::~GCSetItemGoldValue()
{

}

bool GCSetItemGoldValue::Update(const float ticks)
{
	CommandMgr::Instance().GetItemParams()->m_goldValue = m_value;
	return true;
}

//////////////////////
// GCAddItemRange	//
//////////////////////

GCAddItemRange::GCAddItemRange(const int range) :
	GameCommand(CommandID::AddItemRange),
	m_range(range)
{

}

GCAddItemRange::~GCAddItemRange()
{

}

bool GCAddItemRange::Update(const float ticks)
{
	CommandMgr::Instance().GetItemParams()->m_range.push_back(m_range);
	return true;
}

////////////////////////
// GCAddItemEquipable //
////////////////////////

GCAddItemEquipable::GCAddItemEquipable(const std::string& equipable) :
	GameCommand(CommandID::AddItemEquipable),
	m_equipable(equipable)
{

}

GCAddItemEquipable::~GCAddItemEquipable()
{

}

bool GCAddItemEquipable::Update(const float ticks)
{
	CommandMgr::Instance().GetItemParams()->
		m_equipable.push_back(CharacterClass::Convert::NameToEnum(m_equipable.c_str()));
	return true;
}

//////////////////////////////////
	// Normal Mode Commands //     
//////////////////////////////////

//////////////////////
// GCDialogueWindow	//
//////////////////////

GCDialogueWindow::GCDialogueWindow(const std::string& text) :
	GameCommand(CommandID::DialogueWindow),
	m_text(text)
{

}

GCDialogueWindow::~GCDialogueWindow()
{

}

bool GCDialogueWindow::Update(const float ticks)
{
	WindowMgr::Instance().AddDialogueGrp(m_text);
	return true;
}

//////////////////////
// GCFaceWindow		//
//////////////////////

GCFaceWindow::GCFaceWindow(const std::string& text, Actor* const pActor) :
	GameCommand(CommandID::FaceWindow),
	m_text(text),
	m_pActor(pActor)
{

}

GCFaceWindow::~GCFaceWindow()
{

}

bool GCFaceWindow::Update(const float ticks)
{
	WindowMgr::Instance().AddTalkingHeadsGrp(m_pActor, m_text);
	return true;
}

//////////////////////
// GCFlagNeeded		//
//////////////////////

GCFlagNeeded::GCFlagNeeded(const std::string& flag) :
	GameCommand(CommandID::FlagNeeded),
	m_flag(flag)
{

}

GCFlagNeeded::~GCFlagNeeded()
{

}

bool GCFlagNeeded::Update(const float ticks)
{
	if (GameState::Instance().GetFlag(m_flag))
		return true;
	else
		CommandMgr::Instance().GoToNextEnd();

	return true;
}

//////////////////////
// GCNoFlag			//
//////////////////////

GCNoFlag::GCNoFlag(const std::string& flag) :
	GameCommand(CommandID::NoFlag),
	m_flag(flag)
{

}

GCNoFlag::~GCNoFlag()
{

}

bool GCNoFlag::Update(const float ticks)
{
	if (!(GameState::Instance().GetFlag(m_flag)))
		return true;
	else
		CommandMgr::Instance().GoToNextEnd();

	return true;
}

//////////////////////
// GCSetFlag		//
//////////////////////

GCSetFlag::GCSetFlag(const std::string& flag) :
	GameCommand(CommandID::SetFlag),
	m_flag(flag)
{

}

GCSetFlag::~GCSetFlag()
{

}

bool GCSetFlag::Update(const float ticks)
{
	GameState::Instance().SetFlag(m_flag);
	return true;
}

//////////////////////
// GCRemoveFlag		//
//////////////////////

GCRemoveFlag::GCRemoveFlag(const std::string& flag) :
	GameCommand(CommandID::RemoveFlag),
	m_flag(flag)
{

}

GCRemoveFlag::~GCRemoveFlag()
{

}

bool GCRemoveFlag::Update(const float ticks)
{
	GameState::Instance().RemoveFlag(m_flag);
	return true;
}

//////////////////////
// GCEnd			//
//////////////////////

GCEnd::GCEnd() :
	GameCommand(CommandID::End)
{

}

GCEnd::~GCEnd()
{

}

bool GCEnd::Update(const float ticks)
{
	return true;
}

//////////////////////
// GCCheckDirection	//
//////////////////////

GCCheckDirection::GCCheckDirection(const Direction::Type dir) :
	GameCommand(CommandID::CheckDirection),
	m_dir(dir)
{

}

GCCheckDirection::~GCCheckDirection()
{

}

bool GCCheckDirection::Update(const float ticks)
{
	if (ActorMgr::Instance().GetActivePlayer().GetFacing() == m_dir)
		return true;
	else
		CommandMgr::Instance().GoToNextEnd();

	return true;
}

//////////////////////
// GCToggleSolid	//
//////////////////////

GCToggleSolid::GCToggleSolid(const int x, const int y) :
	GameCommand(CommandID::ToggleSolid),
	m_x(x),
	m_y(y)
{

}

GCToggleSolid::~GCToggleSolid()
{

}

bool GCToggleSolid::Update(const float ticks)
{
	GameObjectMgr::Instance().GetTileMap().SetPosSolid(Position(m_x, m_y));
	return true;
}

/////////////////////////
// GCSetBackgroundTile //
/////////////////////////

GCSetBackgroundTile::GCSetBackgroundTile(const int tilenum, const int x, const int y) :
	GameCommand(CommandID::SetBackgroundTile),
	m_tilenum(tilenum),
	m_x(x),
	m_y(y)
{

}

GCSetBackgroundTile::~GCSetBackgroundTile()
{

}

bool GCSetBackgroundTile::Update(const float ticks)
{
	GameObjectMgr::Instance().GetTileMap().GetLayer(0).SetTile(m_tilenum, m_x, m_y);
	return true;
}

//////////////////////
// GCSetOverlayTile	//
//////////////////////

GCSetOverlayTile::GCSetOverlayTile(const int tilenum, const int x, const int y) :
	GameCommand(CommandID::SetOverlayTile),
	m_tilenum(tilenum),
	m_x(x),
	m_y(y)
{

}

GCSetOverlayTile::~GCSetOverlayTile()
{

}

bool GCSetOverlayTile::Update(const float ticks)
{
	GameObjectMgr::Instance().GetTileMap().GetLayer(1).SetTile(m_tilenum, m_x, m_y);
	return true;
}

/////////////////////////
// GCSetForegroundTile //
/////////////////////////

GCSetForegroundTile::GCSetForegroundTile(const int tilenum, const int x, const int y) :
	GameCommand(CommandID::SetForegroundTile),
	m_tilenum(tilenum),
	m_x(x),
	m_y(y)
{

}

GCSetForegroundTile::~GCSetForegroundTile()
{

}

bool GCSetForegroundTile::Update(const float ticks)
{
	GameObjectMgr::Instance().GetTileMap().GetLayer(2).SetTile(m_tilenum, m_x, m_y);
	return true;
}

///////////////////
// GCAddItem	 //
///////////////////

GCAddItem::GCAddItem(const std::string& itemname, const std::string& searchtext) :
	m_itemName(itemname),
	m_searchText(searchtext)
{

}

GCAddItem::~GCAddItem()
{

}

bool GCAddItem::Update(const float ticks)
{
	ActorList partyList = ActorMgr::Instance().GetPartyList();
	const std::string leaderName = ActorMgr::Instance().GetActivePlayer().GetStatBlock().GetName();
	Item item = ItemDB::Instance().GetItem(m_itemName);

	//@TODO - better than before, not hard coded to chests, but can I do better?
	// for instance, still need a way to put the item back in the chest if there is no
	// room for anyone in the party to hold the item
	std::stringstream temp;
	temp << leaderName << " " << m_searchText << "^";
	temp << leaderName << " found\n" << item.GetName().c_str() << ".^";

	for (unsigned int i = 0; i < partyList.size(); ++i)
	{
		if (partyList[i]->GetStatBlock().AddItem(m_itemName))
		{
			if (i == 0)
			{
				temp << leaderName << " recieved\n" << m_itemName.c_str() << "."; 
			}
			else
			{
				temp << leaderName << " passed the\n" << m_itemName.c_str() 
					 << " to " << partyList[i]->GetStatBlock().GetName() << ".";
			}
			break;
		}
		if (i >= (partyList.size()-1))
		{
			temp << "But, everyone's hands are full.";
		}
	}

	WindowMgr::Instance().AddDialogueGrp(temp.str());
	/* Possible outcomes:
	DialogueWindow "BOWIE opened the chest.^BOWIE found\nITEM.^BOWIE recieved\nITEM."
	DialogueWindow "BOWIE opened the chest.^BOWIE found\nITEM.^BOWIE passed the\nITEM to OTHERMEMBER."
	DialogueWindow "BOWIE opened the chest.^BOWIE found\nITEM.^But, everyone's hands are full."
	*/

	return true;
}

///////////////////
// GCSetEvent	 //
///////////////////

GCSetEvent::GCSetEvent(const int eventnum, const int x, const int y) :
	GameCommand(CommandID::SetEvent),
	m_eventNum(eventnum),
	m_x(x),
	m_y(y)
{
	
}

GCSetEvent::~GCSetEvent()
{

}

bool GCSetEvent::Update(const float ticks)
{
	GameObjectMgr::Instance().GetTileMap().SetEventAtPos(Position(m_x, m_y), m_eventNum);
	return true;
}

///////////////////
// GCFadeIn		 //
///////////////////

GCFadeIn::GCFadeIn() :
	GameCommand(CommandID::FadeIn),
	m_ticks(0)
{

}

GCFadeIn::~GCFadeIn()
{

}

bool GCFadeIn::Update(const float ticks)
{
	if (m_ticks == 0)
	{
		GameObjectMgr::Instance().AddObject(m_fader);
		GameObjectMgr::Instance().FocusOnObject(&m_fader);
		m_fader.FadeIn();
	}

	m_ticks += ticks;

	if (m_fader.DoneFadingIn())
	{
		GameObjectMgr::Instance().PopFocusObject();
		GameObjectMgr::Instance().RemoveObject(m_fader);
		return true;
	}
	else
	{
		return false;
	}

}

///////////////////
// GCFadeOut	 //
///////////////////

GCFadeOut::GCFadeOut() :
	GameCommand(CommandID::FadeOut),
	m_ticks(0)
{

}

GCFadeOut::~GCFadeOut()
{

}

bool GCFadeOut::Update(const float ticks)
{
	if (m_ticks == 0)
	{
		GameObjectMgr::Instance().AddObject(m_fader);
		GameObjectMgr::Instance().FocusOnObject(&m_fader);
		m_fader.FadeOut();
	}

	m_ticks += ticks;

	if (m_fader.DoneFadingOut())
	{
		GameObjectMgr::Instance().PopFocusObject();
		GameObjectMgr::Instance().RemoveObject(m_fader);
		return true;
	}
	else
	{
		return false;
	}

}

///////////////////
// GCWarp		 //
///////////////////

GCWarp::GCWarp(const int x, const int y, const std::string& mapname, 
			   const Direction::Type facing) :
	GameCommand(CommandID::Warp),
	m_x(x),
	m_y(y),
	m_mapname(mapname),
	m_facing(facing),
	m_ticks(0)
{

}

GCWarp::~GCWarp()
{

}

bool GCWarp::Update(const float ticks)
{
	if (m_ticks == 0)
	{
		GameObjectMgr::Instance().AddObject(m_fader);
		GameObjectMgr::Instance().FocusOnObject(&m_fader);
		m_fader.FadeOut();
	}

	m_ticks += ticks;

	if (m_fader.DoneFadingOut())
	{
		if (m_mapname != FilePathMgr::Instance().GetCurrMapName())
		{
			GameObjectMgr::Instance().LoadMap(m_mapname);
		}

		Actor& player = ActorMgr::Instance().GetActivePlayer();
		TileMap& map = GameObjectMgr::Instance().GetTileMap();
//		Camera& cam = GameObjectMgr::Instance().GetCamera();
		Position srcPos = player.GetLocalPos().CoordToTileNum();
		Position destPos = Position(m_x, m_y);
		player.SetTilePos(destPos);
		player.SetFacing(m_facing);
		map.SetActorAtPos(srcPos, NULL);
		map.SetActorAtPos(destPos, &player);
		player.ClearMovementState();
//		cam.SnapToActor();

		GameObjectMgr::Instance().PopFocusObject();
		GameObjectMgr::Instance().RemoveObject(m_fader);
		CommandMgr::Instance().AddCommandManually(new GCFadeIn());
		return true;
	}
	else
	{
		return false;
	}
}

///////////////////
// GCWait		 //
///////////////////

GCWait::GCWait(const int delayInMS) :
	GameCommand(CommandID::Wait),
	m_ticks(0),
	m_delay(static_cast<float>(delayInMS))
{
	
}

GCWait::~GCWait()
{

}

bool GCWait::Update(const float ticks)
{
	m_ticks += ticks;
	if (m_ticks >= m_delay)
		return true;
	else
		return false;
}

/////////////////
// GCPlayMusic //
/////////////////

GCPlayMusic::GCPlayMusic(const std::string& filename) :
	GameCommand(CommandID::PlayMusic),
	m_filename(filename)
{

}

GCPlayMusic::~GCPlayMusic()
{

}

bool GCPlayMusic::Update(const float ticks)
{
	AudioMgr::PlaySong(m_filename);
	return true;
}

/////////////////
// GCPlaySound //
/////////////////

GCPlaySound::GCPlaySound(const std::string& filename) :
	GameCommand(CommandID::PlaySound),
	m_filename(filename)
{

}

GCPlaySound::~GCPlaySound()
{

}

bool GCPlaySound::Update(const float ticks)
{
	AudioMgr::PlaySoundEffect(m_filename);
	return true;
}

///////////////////
// GCStartBattle //
///////////////////

GCStartBattle::GCStartBattle() :
	GameCommand(CommandID::StartBattle)
{

}

GCStartBattle::~GCStartBattle()
{

}

bool GCStartBattle::Update(const float ticks)
{
	//@HACK: this probably should not be calling GameEngine directly, but it's the quickest
	// way to talk to everything at the correct level right now.
	GameEngine::Instance().StartBattle();
	return true;
}

///////////////////
// GCEndBattle	 //
///////////////////

GCEndBattle::GCEndBattle() :
	GameCommand(CommandID::EndBattle)
{
	
}

GCEndBattle::~GCEndBattle()
{

}

bool GCEndBattle::Update(const float ticks)
{
	//@HACK: this probably should not be calling GameEngine directly, but it's the quickest
	// way to talk to everything at the correct level right now.
	GameEngine::Instance().EndBattle();
	return true;
}

////////////////////////
// GCShowOutroAndQuit //
////////////////////////

GCShowOutroAndQuit::GCShowOutroAndQuit() :
	GameCommand(CommandID::ShowOutroAndQuit)
{
	
}

GCShowOutroAndQuit::~GCShowOutroAndQuit()
{

}

bool GCShowOutroAndQuit::Update(const float ticks)
{
	static GameOutro exit;
	GameObjectMgr::Instance().AddObject(exit);
	GameObjectMgr::Instance().FocusOnObject(&exit);
	return true;
}

////////////////////
// GCCreateNewMap //
////////////////////

GCCreateNewMap::GCCreateNewMap(const std::string& mapname, const int width, const int height) :
	GameCommand(CommandID::CreateNewMap),
	m_mapname(mapname),
	m_width(width),
	m_height(height)
{

}

GCCreateNewMap::~GCCreateNewMap()
{

}

bool GCCreateNewMap::Update(const float ticks)
{
	TileMap::CreateNewMap(m_mapname, m_width, m_height);
	return true;
}


///////////////////
// GCTileFill	 //
///////////////////

GCTileFill::GCTileFill(const int tilenum, const int layer, const int x1, const int y1, 
					   const int x2, const int y2) :
	GameCommand(CommandID::TileFill),
	m_tilenum(tilenum), 
	m_layer(layer),
	m_x1(x1),
	m_y1(y1),
	m_x2(x2),
	m_y2(y2)
{
	
}

GCTileFill::~GCTileFill()
{

}

bool GCTileFill::Update(const float ticks)
{
	GameObjectMgr::Instance().GetTileMap().TileFill(m_tilenum, m_layer, m_x1, m_y1, m_x2, m_y2);
	return true;
}