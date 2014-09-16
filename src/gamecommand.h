#ifndef _gamecommand_h_
#define _gamecommand_h_

#include "core.h"
#include "fader.h"
#include "itemset.h"
class Actor;

namespace CommandMode
{
	enum Type { Normal, ActorSetup, ItemSetup, MapEditing, NumTypes };

	extern const char* Names[];

	typedef EnumNameConverterT<Type, NumTypes, Names> Convert;
}

namespace CommandID
{
	enum Type { NoCommand, 
				// Mode Shifting Commands
				SetCommandMode, ResetCommandMode,
				// ActorSetup Mode Only Commands
				BeginNewActor, EndNewActor,
				SetActorClass, SetActorMobility, SetActorStats,
				SetActorTileset, SetActorFaceset, SetActorBattleset,
				SetActorPosition, SetActorFacing,
				SetActorSpell, SetActorItem, SetActorItemEquipped,
				PlaceAlly, AddPathStep,
				// ItemSetup Mode Only Commands
				BeginNewItem, EndNewItem,
				SetItemClass, SetItemIcon, SetCutsceneGfxFilename,
				SetItemStats, SetItemGoldValue,
				AddItemRange, AddItemEquipable,
				// Normal Mode Only Commands
				DialogueWindow, FaceWindow, 
				FlagNeeded, NoFlag, SetFlag, RemoveFlag, End,
				CheckDirection,
				ToggleSolid, SetBackgroundTile, SetOverlayTile, SetForegroundTile, 
				AddItem, 
				SetEvent, 
				FadeIn, FadeOut,
				Warp, 
				Wait, 
				PlayMusic, PlaySound,
				StartBattle, EndBattle,
				ShowOutroAndQuit,
				// MapEditing Mode Only Commands
				CreateNewMap, TileFill,
				NumTypes };
	// Commands that still need implemented:
	/*
				Counter(top) - talk to NPCs across shop counters and tables
				YesNoWindow - branch based on player y/n input
	*/
	// Ideas for New Commands!
	/*
				OnSuccess, OnFailure - Let other commands essentially have a return value and
									   these commands can run depending on a preceeding
									   script's results.  This could be a solution to the
									   treasure chest problem where the item needs to stay in
									   the chest and the lid needs to close back up if the
									   entire party's inventory is full. 
				OnSearch(working name) - hidden items or events that only fire if the player
										 uses the Search command
	*/
}

// Base GameCommand class
class GameCommand
{
public:
	GameCommand(const CommandID::Type id = CommandID::NoCommand);
	virtual ~GameCommand();
	CommandID::Type GetID();
	virtual bool Update(const float ticks);

protected:
	CommandID::Type m_commandID;
};

////////////////////////////
// Mode Shifting Commands //
////////////////////////////

// SetCommandMode - Changes what mode the commandmgr is in.  Certain commands only work in 
// certain modes.  Ex: Warp only works in Normal mode, ActorSetup commands only work in 
// ActorSetup mode, etc.
class GCSetCommandMode : public GameCommand
{
public:
	GCSetCommandMode(const std::string& mode);
	virtual ~GCSetCommandMode();
	virtual bool Update(const float ticks);

private:
	std::string m_comMode;
};

// GCResetCommandMode - reverts to Normal command mode.
class GCResetCommandMode : public GameCommand
{
public:
	GCResetCommandMode();
	virtual ~GCResetCommandMode();
	virtual bool Update(const float ticks);

private:

};

//////////////////////////////
// ActorSetup Mode Commands //
//////////////////////////////

// BeginNewActor - Starts an actor block and sets the name of the actor about to be created
class GCBeginNewActor : public GameCommand
{
public:
	GCBeginNewActor(const std::string& name, const std::string& team);
	virtual ~GCBeginNewActor();
	virtual bool Update(const float ticks);

private:
	std::string m_name;
	std::string m_team;
};

// EndNewActor - Finishes actor setup and creates the specified actor
class GCEndNewActor : public GameCommand
{
public:
	GCEndNewActor();
	virtual ~GCEndNewActor();
	virtual bool Update(const float ticks);

private:

};

// SetActorClass - Sets the class of the actor about to be created
class GCSetActorClass : public GameCommand
{
public:
	GCSetActorClass(const std::string& actorclass);
	virtual ~GCSetActorClass();
	virtual bool Update(const float ticks);

private:
	std::string m_class;
};

// SetActorMobility - Sets the terrain mobility of the actor about to be created
class GCSetActorMobility : public GameCommand
{
public:
	GCSetActorMobility(const std::string& mobility);
	virtual ~GCSetActorMobility();
	virtual bool Update(const float ticks);

private:
	std::string m_mobility;
};

// SetActorStats - Sets the level, hp, mp, exp, atk, def, agi, and mov of the actor
// about to be created
class GCSetActorStats : public GameCommand
{
public:
	GCSetActorStats(const int level, const int hp, const int mp, const int exp, 
					const int atk, const int def, const int agi, const int mov);
	virtual ~GCSetActorStats();
	virtual bool Update(const float ticks);

private:
	int m_level, m_hp, m_mp, m_exp, m_atk, m_def, m_agi, m_mov;
};

// SetActorTileset - Sets the tileset of the actor about to be created
class GCSetActorTileset : public GameCommand
{
public:
	GCSetActorTileset(const std::string& tsname);
	virtual ~GCSetActorTileset();
	virtual bool Update(const float ticks);

private:
	std::string m_tsname;
};

// SetActorFaceset - Sets the faceset of the actor about to be created
class GCSetActorFaceset : public GameCommand
{
public:
	GCSetActorFaceset(const std::string& fsname);
	virtual ~GCSetActorFaceset();
	virtual bool Update(const float ticks);

private:
	std::string m_fsname;
};

// SetActorBattleset - Sets the filename of the battle animation frames, and the number
// of frames in that set.
class GCSetActorBattleset : public GameCommand
{
public:
	GCSetActorBattleset(const std::string& bsname, const int numframes);
	virtual ~GCSetActorBattleset();
	virtual bool Update(const float ticks);

private:
	std::string m_bsname;
	int m_numFrames;
};

// SetActorPosition - Sets the initial position of the actor about to be created.
// NOTE: This is for setup ONLY, not valid to change an actor's position after it is created!
class GCSetActorPosition : public GameCommand
{
public:
	GCSetActorPosition(const int x, const int y);
	virtual ~GCSetActorPosition();
	virtual bool Update(const float ticks);

private:
	int m_x, m_y;
};

// SetActorFacing - Sets the facing of the actor about to be created
// NOTE: This is for setup ONLY, not valid to change an actor's facing after it is created!
class GCSetActorFacing : public GameCommand
{
public:
	GCSetActorFacing(const std::string& facing);
	virtual ~GCSetActorFacing();
	virtual bool Update(const float ticks);

private:
	std::string m_facing;
};

// SetActorSpell - Puts a spell into an actors spellbook at creation only
class GCSetActorSpell : public GameCommand
{
public:
	GCSetActorSpell(const std::string& spell);
	virtual ~GCSetActorSpell();
	virtual bool Update(const float ticks);

private:
	std::string m_spell;
};

// SetActorItem - Puts an item into an actors inventory at creation only
class GCSetActorItem : public GameCommand
{
public:
	GCSetActorItem(const std::string& item);
	virtual ~GCSetActorItem();
	virtual bool Update(const float ticks);

private:
	std::string m_item;
};

// SetActorItemEquipped - Puts an item into an actor's inventory at creation only and equips it
//@TODO: Currently only works on weapons, and does not check for if the actor's class can
// equip the selected item, so it is on the script writer to use this responsibly!
// Although, this could also be convenient for "equipping" weapons to enemies which will not
// have classes like player characters.
class GCSetActorItemEquipped : public GameCommand
{
public:
	GCSetActorItemEquipped(const std::string& item);
	virtual ~GCSetActorItemEquipped();
	virtual bool Update(const float ticks);

private:
	std::string m_item;
};

// PlaceAlly - Queues up an already created party member in the active force to be 
// placed on the battlefield when battle starts.  This queue of placements is cleared when the
// next battle ends.  Allies are placed in order from the ActorMgr::m_players[1] actor list
// on down the line for each call to this, m_players[0] being the party leader needs to be 
// placed via Warp onto the map.
class GCPlaceAlly : public GameCommand
{
public:
	GCPlaceAlly(const int x, const int y);
	virtual ~GCPlaceAlly();
	virtual bool Update(const float ticks);

private:
	int m_x, m_y;
};

// AddPathStep
class GCAddPathStep : public GameCommand
{
public:
	GCAddPathStep(const std::string& dir);
	virtual ~GCAddPathStep();
	virtual bool Update(const float ticks);

private:
	std::string m_direction;
};

/////////////////////////////
// ItemSetup Mode Commands //
/////////////////////////////

// BeginNewItem - Starts an item block and sets the name of the item about to be created
class GCBeginNewItem : public GameCommand
{
public:
	GCBeginNewItem(const std::string& name);
	virtual ~GCBeginNewItem();
	virtual bool Update(const float ticks);

private:
	std::string m_name;
};

// EndNewItem - Finishes item setup and creates the specified item
class GCEndNewItem : public GameCommand
{
public:
	GCEndNewItem();
	virtual ~GCEndNewItem();
	virtual bool Update(const float ticks);

private:

};

// SetItemClass - Sets the item's class (Weapon, Accessory, etc)
class GCSetItemClass : public GameCommand
{
public:
	GCSetItemClass(const std::string& itemclass);
	virtual ~GCSetItemClass();
	virtual bool Update(const float ticks);

private:
	std::string m_itemClass;
};

// SetItemIcon - Sets the item's ItemGfxID attribute
class GCSetItemIcon : public GameCommand
{
public:
	GCSetItemIcon(const std::string& icon);
	virtual ~GCSetItemIcon();
	virtual bool Update(const float ticks);

private:
	std::string m_itemIcon;
};

// SetCutsceneGfxFilename - Sets the filename of the item's cutscene graphic, typically 
// used for weapons only
class GCSetCutsceneGfxFilename : public GameCommand
{
public:
	GCSetCutsceneGfxFilename(const std::string& filename);
	virtual ~GCSetCutsceneGfxFilename();
	virtual bool Update(const float ticks);

private:
	std::string m_filename;
};

// SetItemStats - Sets which stats the item modifies.  Equipment will modify stats while the
// item is equipped, consumables (Quick Chicken, Brave Apple, etc) will boost the specified 
// stats permanently.
class GCSetItemStats : public GameCommand
{
public:
	GCSetItemStats(const int hp, const int mp, const int atk, 
				   const int def, const int agi, const int mov);
	virtual ~GCSetItemStats();
	virtual bool Update(const float ticks);

private:
	int m_hp, m_mp, m_atk, m_def, m_agi, m_mov;
};

// SetItemGoldValue - Sets the value an item is worth when purchased from a merchant.
class GCSetItemGoldValue : public GameCommand
{
public:
	GCSetItemGoldValue(const int value);
	virtual ~GCSetItemGoldValue();
	virtual bool Update(const float ticks);

private:
	int m_value;
};

// AddItemRange - Adds a range bracket to an item.  Melee weapons will be 1 only, simple
// healing items like medical herbs will get both 0 (self) and 1, ammo for bows/guns will
// get 2 or greater but not 1, and so on.
class GCAddItemRange : public GameCommand
{
public:
	GCAddItemRange(const int range);
	virtual ~GCAddItemRange();
	virtual bool Update(const float ticks);

private:
	int m_range;
};

// AddItemEquipable - Adds a class that can equip the item to its m_equippable vector
class GCAddItemEquipable : public GameCommand
{
public:
	GCAddItemEquipable(const std::string& equipable);
	virtual ~GCAddItemEquipable();
	virtual bool Update(const float ticks);

private:
	std::string m_equipable;
};

//////////////////////////
// Normal Mode Commands //
//////////////////////////

// DialogueWindow - A standard text window without a face
class GCDialogueWindow : public GameCommand
{
public:
	GCDialogueWindow(const std::string& text);
	virtual ~GCDialogueWindow();
	virtual bool Update(const float ticks);

private:
	std::string m_text;
};

// FaceWindow - A standard text window with a face
//@HACK: need to rethink how to reference facewindows
class GCFaceWindow : public GameCommand
{
public:
	GCFaceWindow(const std::string& text, Actor* const pActor);
	virtual ~GCFaceWindow();
	virtual bool Update(const float ticks);

private:
	std::string m_text;
	Actor* m_pActor;
};

// FlagNeeded - If the specified flag has been set with SetFlag, run the commands in the 
// script between this and the next End command, else skip all commands until the next End.
class GCFlagNeeded : public GameCommand
{
public:
	GCFlagNeeded(const std::string& flag);
	virtual ~GCFlagNeeded();
	virtual bool Update(const float ticks);

private:
	std::string m_flag;
};

// NoFlag - Only run the commands in the script between this and the next End command if the
// specified flag has NOT been set.  If it has, skip all commands until the next End.
class GCNoFlag : public GameCommand
{
public:
	GCNoFlag(const std::string& flag);
	virtual ~GCNoFlag();
	virtual bool Update(const float ticks);

private:
	std::string m_flag;
};

// SetFlag - Set the specified flag in the GameState.  Currently it behaves like a bool and 
// only checks if it is present or not, but the flag map is a pairing of strings and ints, so
// flags will eventually be much more robust.
class GCSetFlag : public GameCommand
{
public:
	GCSetFlag(const std::string& flag);
	virtual ~GCSetFlag();
	virtual bool Update(const float ticks);

private:
	std::string m_flag;
};

// RemoveFlag - If the specified flag has been set, remove it.
class GCRemoveFlag : public GameCommand
{
public:
	GCRemoveFlag(const std::string& flag);
	virtual ~GCRemoveFlag();
	virtual bool Update(const float ticks);

private:
	std::string m_flag;
};

// End - Companion command to FlagNeeded and NoFlag.  Commands between them and an End 
// statement will be run depending on if the specified flag is or is not set.
//@TODO: Should GCEnd contain a second CommandID or some other identifier to pair it with
// its partner, so that nesting works?
class GCEnd : public GameCommand
{
public:
	GCEnd();
	virtual ~GCEnd();
	virtual bool Update(const float ticks);

private:
	
};

// CheckDirection - run code from here to an End based on if the player is facing a
// specified direction
//@TODO: add code to check for/assert valid directions in commandmgr for this command
class GCCheckDirection : public GameCommand
{
public:
	GCCheckDirection(const Direction::Type dir);
	virtual ~GCCheckDirection();
	virtual bool Update(const float ticks);

private:
	Direction::Type m_dir;
};

// ToggleSolid - flips a tile's solid value at a specified x, y position on the current map.
class GCToggleSolid : public GameCommand
{
public:
	GCToggleSolid(const int x, const int y);
	virtual ~GCToggleSolid();
	virtual bool Update(const float ticks);

private:
	int m_x, m_y;
};

// SetBackgroundTile - Sets a tile value at the specified x, y pos on the Background layer.
class GCSetBackgroundTile : public GameCommand
{
public:
	GCSetBackgroundTile(const int tilenum, const int x, const int y);
	virtual ~GCSetBackgroundTile();
	virtual bool Update(const float ticks);

private:
	int m_x, m_y, m_tilenum;
};

// SetOverlayTile - Sets a tile value at the specified x, y pos on the Overlay layer.
class GCSetOverlayTile : public GameCommand
{
public:
	GCSetOverlayTile(const int tilenum, const int x, const int y);
	virtual ~GCSetOverlayTile();
	virtual bool Update(const float ticks);

private:
	int m_x, m_y, m_tilenum;
};

// SetForegroundTile - Sets a tile value at the specified x, y pos on the Foreground layer.
class GCSetForegroundTile : public GameCommand
{
public:
	GCSetForegroundTile(const int tilenum, const int x, const int y);
	virtual ~GCSetForegroundTile();
	virtual bool Update(const float ticks);

private:
	int m_x, m_y, m_tilenum;
};

// AddItem - Attempts to add the specified item to the party's inventory.  Also takes a
// string argument to describe what is searched and how.
//@TODO: if nobody can hold an item when this is run, need a way to not have the event vanish.
// Right now, there's no easy way to tell if this command succeeded or failed, so scripts that
// remove or replace the event after this runs are probably always going to have the player 
// lose their opportunity to get this item forever.
class GCAddItem : public GameCommand
{
public:
	GCAddItem(const std::string& itemname, const std::string& searchtext = " searched the area.");
	virtual ~GCAddItem();
	virtual bool Update(const float ticks);

private:
	std::string m_itemName;
	std::string m_searchText;
};

// SetEvent - sets an event of the specified number at the x, y pos on the current map.
class GCSetEvent : public GameCommand
{
public:
	GCSetEvent(const int eventnum, const int x, const int y);
	virtual ~GCSetEvent();
	virtual bool Update(const float ticks);

private:
	int m_x, m_y, m_eventNum;
};

// FadeIn - Fades the screen in from black.  If it's not black to start, it becomes black, 
// then fades in.
//@TODO: fade rate on these may need tweaked
class GCFadeIn : public GameCommand
{
public:
	GCFadeIn();
	virtual ~GCFadeIn();
	virtual bool Update(const float ticks);

private:
	float m_ticks;
	Fader m_fader;
};

// FadeOut - Fades the screen out to black.
//@TODO: fade rate on these may need tweaked
class GCFadeOut : public GameCommand
{
public:
	GCFadeOut();
	virtual ~GCFadeOut();
	virtual bool Update(const float ticks);

private:
	float m_ticks;
	Fader m_fader;
};

// Warp - fade out, put the player at the specified pos facing a certain direction
//@TODO: Need to be able to either specify to do a fast, fadeless warp or make a new command
// which does that.
class GCWarp : public GameCommand
{
public:
	GCWarp(const int x, const int y, const std::string& mapname, const Direction::Type facing);
	virtual ~GCWarp();
	virtual bool Update(const float ticks);

private:
	int m_x, m_y;
	std::string m_mapname;
	Direction::Type m_facing;
	float m_ticks;
	Fader m_fader;
};

// Wait - pause a number of millisecons.
class GCWait : public GameCommand
{
public:
	GCWait(const int delayInMS);
	virtual ~GCWait();
	virtual bool Update(const float ticks);

private:
	float m_ticks;
	float m_delay;
};

// PlayMusic - Plays a song on repeat.  
//@TODO: Cuts off current music, need to eventually implement fade out and/or cross fade
class GCPlayMusic : public GameCommand
{
public:
	GCPlayMusic(const std::string& filename);
	virtual ~GCPlayMusic();
	virtual bool Update(const float ticks);

private:
	std::string m_filename;
};

// PlaySound - Plays a single sound effect once.
//@TODO: add something like "PlayDelayedSound "foo.ogg" 80" perhaps?
class GCPlaySound : public GameCommand
{
public:
	GCPlaySound(const std::string& filename);
	virtual ~GCPlaySound();
	virtual bool Update(const float ticks);

private:
	std::string m_filename;
};

// Forces the GameModeMgr to switch to Battle mode
class GCStartBattle : public GameCommand
{
public:
	GCStartBattle();
	virtual ~GCStartBattle();
	virtual bool Update(const float ticks);

private:
	
};

// Forces the GameModeMgr to leave Battle mode and return to Field mode
class GCEndBattle : public GameCommand
{
public:
	GCEndBattle();
	virtual ~GCEndBattle();
	virtual bool Update(const float ticks);

private:
	
};

class GCShowOutroAndQuit : public GameCommand
{
public:
	GCShowOutroAndQuit();
	virtual ~GCShowOutroAndQuit();
	virtual bool Update(const float ticks);

private:
	
};

// CreateNewMap - Spawns a new map with the specified name and dimensions only, does not
// automatically switch to the new map, that needs done with the Load Map function of the
// editor or by warping there via script.
class GCCreateNewMap : public GameCommand
{
public:
	GCCreateNewMap(const std::string& mapname, const int width, const int height);
	virtual ~GCCreateNewMap();
	virtual bool Update(const float ticks);

private:
	int m_width, m_height;
	std::string m_mapname;
};

// TileFill - Does a fill on the map from x1, y1 to x2, y2 of the specified tile on the chosen 
// layer.  Must be a valid, in bounds rectangle, or nothing will happen.
class GCTileFill : public GameCommand
{
public:
	GCTileFill(const int tilenum, const int layer, 
			   const int x1, const int y1, const int x2, const int y2);
	virtual ~GCTileFill();
	virtual bool Update(const float ticks);

private:
	int m_tilenum, m_layer, m_x1, m_y1, m_x2, m_y2;
};

#endif //_gamecommand_h_