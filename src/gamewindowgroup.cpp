#include "gamewindowgroup.h"
#include <sstream>
#include "windowmgr.h"
#include "audiomgr.h"
#include "actormgr.h"
#include "gameobjectmgr.h"
#include "statblock.h"
#include "inputmgr.h"
#include "actor.h"

///////////////////////
// GameWindowGroup   //
///////////////////////

GameWindowGroup::GameWindowGroup() :
	GameObject(LayerSpace::Window),
	m_bExpired(false)
{

}

void GameWindowGroup::SummonWindows()
{

}

void GameWindowGroup::DismissWindows()
{
	m_bExpired = true;
}

void GameWindowGroup::DismissWindowsSilently()
{
	m_bExpired = true;
}

bool GameWindowGroup::IsExpired()
{
	return m_bExpired;
}

void GameWindowGroup::ProcessInput()
{
	if (InputMgr::Instance().WasPressed(GEN_A) ||
		InputMgr::Instance().WasPressed(GEN_B) ) 
	{
 		if (!m_bExpired)
		{
			DismissWindows();
		}
	}
}

void GameWindowGroup::DoUpdate(const float ticks)
{

}

///////////////////////
// GWGCharacterStats //
///////////////////////

GWGCharacterStats::GWGCharacterStats(Actor* pActor) : 
	m_statsWin(168, 208, Vector2f(112, 16)),
	m_faceWin(64, 80, Vector2f(48, 16)),
	m_kdWin(64, 128, Vector2f(48, 96)),
	m_pActor(pActor)
{
	assert(m_pActor != NULL);

	m_statsWin.SetFont(FontMgr::Instance().GetFont(FontStyle::FixedWidth));
	m_statsWin.SetText(m_pActor->GetStatBlock().GetStatWindowString());
	m_statsWin.SetSlideDest(CompassDir::E);
	m_statsWin.GetSlider().SnapOffScreen();

	m_invRenderer.SetRenderMode(true);
	m_invRenderer.SetInventory(m_pActor->GetStatBlock().GetInventory());
	m_invRenderer.SetLocalPos(Position(80, 104));
	m_statsWin.AddChild(m_invRenderer);

	m_sbRenderer.SetRenderMode(true);
	m_sbRenderer.SetSpellBook(m_pActor->GetStatBlock().GetSpellBook());
	m_sbRenderer.SetLocalPos(Position(8, 104));
	m_statsWin.AddChild(m_sbRenderer);

	m_faceWin.SetDecal(m_pActor->GetFaceTex());
	m_faceWin.SetSlideDest(CompassDir::NW);
	m_faceWin.GetSlider().SnapOffScreen();

	m_kdWin.SetFont(FontMgr::Instance().GetFont(FontStyle::FixedWidth));
	m_kdWin.SetText(m_pActor->GetStatBlock().GetKillsDefeatsString());
	m_kdWin.SetSlideDest(CompassDir::W);
	m_kdWin.GetSlider().SnapOffScreen();

	AddChild(m_statsWin);
	AddChild(m_faceWin);
	AddChild(m_kdWin);
}

void GWGCharacterStats::SummonWindows()
{
	AudioMgr::PlaySoundEffect("swish1.ogg", 0);
	m_statsWin.GetSlider().SlideOntoScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 80);
	m_faceWin.GetSlider().SlideOntoScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 160);
	m_kdWin.GetSlider().SlideOntoScreen();
}

void GWGCharacterStats::DismissWindows()
{
	AudioMgr::PlaySoundEffect("swish1.ogg", 0);
	m_statsWin.GetSlider().SlideOffScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 80);
	m_faceWin.GetSlider().SlideOffScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 160);
	m_kdWin.GetSlider().SlideOffScreen();
	m_bExpired = true;
}

bool GWGCharacterStats::IsExpired()
{
	if (m_bExpired && m_statsWin.GetSlider().IsOffScreen() &&
		m_faceWin.GetSlider().IsOffScreen() && m_kdWin.GetSlider().IsOffScreen())
		return true;
	else
		return false;
}

//////////////////////
// GWGTeam			//
//////////////////////

GWGTeam::GWGTeam() : 
	m_faceWin(64, 80, Vector2f(48, 16)),
	m_basicStatsWin(168, 104, Vector2f(112, 16)),
	m_charListWin(232, 104, Vector2f(48, 120)),
	m_actorIndex(0),
	m_topRow(0),
	m_viewableRows(5)
{
	ActorList& partylist = ActorMgr::Instance().GetPartyList();
	
	m_faceWin.SetDecal(partylist[0]->GetFaceTex());
	m_faceWin.SetSlideDest(CompassDir::NW);
	m_faceWin.GetSlider().SnapOffScreen();

	m_basicStatsWin.SetFont(FontMgr::Instance().GetFont(FontStyle::FixedWidth));
	m_basicStatsWin.SetText(partylist[0]->GetStatBlock().GetBasicStatsString());
	m_basicStatsWin.SetSlideDest(CompassDir::NE);
	m_basicStatsWin.GetSlider().SnapOffScreen();

	m_sbRenderer.SetRenderMode(false, true);
	m_sbRenderer.SetSpellBook(partylist[0]->GetStatBlock().GetSpellBook());
	m_sbRenderer.SetLocalPos(Position(16, 32));
	m_basicStatsWin.AddChild(m_sbRenderer);

	m_invRenderer.SetRenderMode(false, true);
	m_invRenderer.SetInventoryHolder(partylist[0]);
	m_invRenderer.SetInventory(partylist[0]->GetStatBlock().GetInventory());
	m_invRenderer.SetLocalPos(Position(88, 32));
	m_basicStatsWin.AddChild(m_invRenderer);

	m_teamText.SetFont(FontMgr::Instance().GetFont(FontStyle::FixedWidth));
	m_teamText.SetOffset(8, 24);
	std::string temp;
	for (int i = 0; i < (int)partylist.size(); i++)
	{
		temp += partylist[i]->GetStatBlock().GetTeamRowString();
	}
	m_teamText.SetString(temp);
	m_charListWin.AddChild(m_teamText);

	m_charListWin.SetFont(FontMgr::Instance().GetFont(FontStyle::FixedWidth));
	m_charListWin.SetText(" NAME    CLASS     LEV EXP");
	m_charListWin.SetSlideDest(CompassDir::S);
	m_charListWin.GetSlider().SnapOffScreen();
	m_charListWin.SetDecal(GameWindowTexMgr::Instance().GetBracket());
	m_charListWin.SetDecalPos(Position(13, 20));

	AddChild(m_faceWin);
	AddChild(m_basicStatsWin);
	AddChild(m_charListWin);
}

void GWGTeam::SummonWindows()
{
	m_faceWin.GetSlider().SlideOntoScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 0);
	m_basicStatsWin.GetSlider().SlideOntoScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 80);
	m_charListWin.GetSlider().SlideOntoScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 160);
}

void GWGTeam::DismissWindows()
{
	m_faceWin.GetSlider().SlideOffScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 0);
	m_basicStatsWin.GetSlider().SlideOffScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 80);
	m_charListWin.GetSlider().SlideOffScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 160);
	m_bExpired = true;
}

bool GWGTeam::IsExpired()
{
	if (m_bExpired && m_faceWin.GetSlider().IsOffScreen() &&
		m_basicStatsWin.GetSlider().IsOffScreen() && m_charListWin.GetSlider().IsOffScreen())
		return true;
	else
		return false;
}

void GWGTeam::ProcessInput()
{
	ActorList& partylist = ActorMgr::Instance().GetPartyList();

	if (InputMgr::Instance().WasPressed(GEN_A)) 
	{
		DismissWindows();
		WindowMgr::Instance().AddCharStatsWinGrp(partylist[m_actorIndex]);
	}

	if (InputMgr::Instance().WasPressed(GEN_B)) 
	{
		DismissWindows();
	}

	// do not accept up or down if the text is still moving
	if (m_teamText.IsScrolling())
		return;

	if (InputMgr::Instance().WasPressed(VK_UP) && (m_actorIndex > 0))
	{
		m_actorIndex--;
		if (m_actorIndex < m_topRow)
		{
			m_topRow--;
			m_teamText.ScrollLine(Direction::Down, 2);
		}
		else
			m_charListWin.MoveDecal(0, -16);

		m_invRenderer.SetInventory(partylist[m_actorIndex]->GetStatBlock().GetInventory());
		m_sbRenderer.SetSpellBook(partylist[m_actorIndex]->GetStatBlock().GetSpellBook());
		m_basicStatsWin.SetText(partylist[m_actorIndex]->GetStatBlock().GetBasicStatsString());
		m_faceWin.SetDecal(partylist[m_actorIndex]->GetFaceTex());
	}

	if (InputMgr::Instance().WasPressed(VK_DOWN) && (m_actorIndex < static_cast<int>(partylist.size()-1)))
	{
		m_actorIndex++;
		if ((m_actorIndex - m_topRow) >= m_viewableRows)
		{
			m_topRow++;
			m_teamText.ScrollLine(Direction::Up, 2);
		}
		else
			m_charListWin.MoveDecal(0, 16);

		m_invRenderer.SetInventory(partylist[m_actorIndex]->GetStatBlock().GetInventory());
		m_sbRenderer.SetSpellBook(partylist[m_actorIndex]->GetStatBlock().GetSpellBook());
		m_basicStatsWin.SetText(partylist[m_actorIndex]->GetStatBlock().GetBasicStatsString());
		m_faceWin.SetDecal(partylist[m_actorIndex]->GetFaceTex());
	}
}

void GWGTeam::DoUpdate(const float ticks)
{
	Vector2f pos = m_charListWin.GetLocalPos();
	m_teamText.SetBounds(static_cast<int>(pos.x + 0.5f), static_cast<int>(pos.y + 12.5f), 224, 92);
}

//////////////////////
// GWGTeamMagic		//
//////////////////////

GWGTeamMagic::GWGTeamMagic()
{
	ActorList& partylist = ActorMgr::Instance().GetPartyList();

	m_sbRenderer.SetRenderMode();
	m_sbRenderer.SetSpellBook(partylist[0]->GetStatBlock().GetSpellBook());
	m_sbRenderer.SetLocalPos(Position(16, 40));
	m_basicStatsWin.AddChild(m_sbRenderer);
	m_basicStatsWin.RemoveChild(m_invRenderer);

	m_basicStatsWin.SetText(partylist[0]->GetStatBlock().GetMagicStatsString());
}

void GWGTeamMagic::ProcessInput()
{
	ActorList& partylist = ActorMgr::Instance().GetPartyList();

	if (InputMgr::Instance().WasPressed(GEN_A)) 
	{
		// NYI
	}

	if (InputMgr::Instance().WasPressed(GEN_B)) 
	{
		DismissWindows();
	}

	// do not accept up or down if the text is still moving
	if (m_teamText.IsScrolling())
		return;

	if (InputMgr::Instance().WasPressed(VK_UP) && (m_actorIndex > 0))
	{
		m_actorIndex--;
		if (m_actorIndex < m_topRow)
		{
			m_topRow--;
			m_teamText.ScrollLine(Direction::Down, 2);
		}
		else
			m_charListWin.MoveDecal(0, -16);

		m_sbRenderer.SetSpellBook(partylist[m_actorIndex]->GetStatBlock().GetSpellBook());
		m_basicStatsWin.SetText(partylist[m_actorIndex]->GetStatBlock().GetMagicStatsString());
		m_faceWin.SetDecal(partylist[m_actorIndex]->GetFaceTex());
	}

	if (InputMgr::Instance().WasPressed(VK_DOWN) && (m_actorIndex < (int)(partylist.size()-1)))
	{
		m_actorIndex++;
		if (m_actorIndex >= m_viewableRows)
		{
			m_topRow++;
			m_teamText.ScrollLine(Direction::Up, 2);
		}
		else
			m_charListWin.MoveDecal(0, 16);

		m_sbRenderer.SetSpellBook(partylist[m_actorIndex]->GetStatBlock().GetSpellBook());
		m_basicStatsWin.SetText(partylist[m_actorIndex]->GetStatBlock().GetMagicStatsString());
		m_faceWin.SetDecal(partylist[m_actorIndex]->GetFaceTex());
	}
}

//////////////////////
// GWGTeamItem		//
//////////////////////

GWGTeamItem::GWGTeamItem() :
	m_equipWpnWin(168, 104, Vector2f(112, 16)),
	m_equipAccWin(168, 104, Vector2f(112, 16)),
	m_selection(0)
{
	ActorList& partylist = ActorMgr::Instance().GetPartyList();

	m_invRenderer.SetRenderMode();
	m_invRenderer.SetInventory(partylist[0]->GetStatBlock().GetInventory());
	m_invRenderer.SetLocalPos(Position(16, 40));
	m_basicStatsWin.AddChild(m_invRenderer);
	m_basicStatsWin.RemoveChild(m_sbRenderer);
	m_basicStatsWin.SetText(partylist[0]->GetStatBlock().GetItemStatsString());
	m_equipWpnWin.SetFont(FontMgr::Instance().GetFont(FontStyle::FixedWidth));
	m_equipWpnWin.SetText(partylist[0]->GetStatBlock().GetEquipHeaderString());
	m_equipWpnWin.SetSlideDest(CompassDir::E);
	m_equipWpnWin.GetSlider().SnapOffScreen();
//	m_equipAccWin.SetText(partylist[0]->GetStatBlock().GetEquipHeaderString());

	AddChild(m_equipWpnWin);
}

void GWGTeamItem::ProcessInput()
{
	ActorList& partylist = ActorMgr::Instance().GetPartyList();

	// Process input for root of Equip windows
	if (!m_basicStatsWin.GetSlider().IsOffScreen())
	{
		if (InputMgr::Instance().WasPressed(GEN_A))
		{
			m_basicStatsWin.SetSlideDest(CompassDir::E);
			m_basicStatsWin.GetSlider().SlideOffScreen();
			AudioMgr::PlaySoundEffect("swish1.ogg", 0);
			m_equipWpnWin.GetSlider().SlideOntoScreen();
			m_equipWpnWin.AddChild(m_invRenderer);
			m_equipWpnWin.SetText(partylist[m_actorIndex]->GetStatBlock().GetEquipHeaderString());
			m_invRenderer.SetFilter(InventoryFilter::Weapons);
			m_invRenderer.ShowItemStats();
			AudioMgr::PlaySoundEffect("swish1.ogg", 80);
		}

		if (InputMgr::Instance().WasPressed(GEN_B))
		{
			m_basicStatsWin.SetSlideDest(CompassDir::NE);
			DismissWindows();
		}

		// do not accept up or down if the text is still moving
		if (m_teamText.IsScrolling())
			return;

		if (InputMgr::Instance().WasPressed(VK_UP) && (m_actorIndex > 0))
		{
			m_actorIndex--;
			if (m_actorIndex < m_topRow)
			{
				m_topRow--;
				m_teamText.ScrollLine(Direction::Down, 2);
			}
			else
				m_charListWin.MoveDecal(0, -16);

			m_invRenderer.SetInventory(partylist[m_actorIndex]->GetStatBlock().GetInventory());
			m_basicStatsWin.SetText(partylist[m_actorIndex]->GetStatBlock().GetItemStatsString());
			m_invRenderer.SetInventoryHolder(partylist[m_actorIndex]);
			m_faceWin.SetDecal(partylist[m_actorIndex]->GetFaceTex());
		}

		if (InputMgr::Instance().WasPressed(VK_DOWN) && (m_actorIndex < static_cast<int>(partylist.size()-1)))
		{
			m_actorIndex++;
			if ((m_actorIndex - m_topRow) >= m_viewableRows)
			{
				m_topRow++;
				m_teamText.ScrollLine(Direction::Up, 2);
			}
			else
				m_charListWin.MoveDecal(0, 16);

			m_invRenderer.SetInventory(partylist[m_actorIndex]->GetStatBlock().GetInventory());
			m_basicStatsWin.SetText(partylist[m_actorIndex]->GetStatBlock().GetItemStatsString());
			m_invRenderer.SetInventoryHolder(partylist[m_actorIndex]);
			m_faceWin.SetDecal(partylist[m_actorIndex]->GetFaceTex());
		}
	}

	// Process input for Equip subwindow
	else // m_equipWpnWin is onscreen
	{
		if (InputMgr::Instance().WasPressed(GEN_B)) 
		{
			m_equipWpnWin.GetSlider().SlideOffScreen();
			AudioMgr::PlaySoundEffect("swish1.ogg", 0);
			m_basicStatsWin.GetSlider().SlideOntoScreen();
			m_basicStatsWin.AddChild(m_invRenderer);
			m_invRenderer.SetChoice(Direction::Up);
			m_invRenderer.SetFilter(InventoryFilter::All);
			m_invRenderer.ShowItemList();
			AudioMgr::PlaySoundEffect("swish1.ogg", 80);
		}

		if (InputMgr::Instance().WasPressed(GEN_A))
		{
			m_invRenderer.EquipSelection();
			m_equipWpnWin.GetSlider().SlideOffScreen();
			AudioMgr::PlaySoundEffect("swish1.ogg", 0);
			m_basicStatsWin.GetSlider().SlideOntoScreen();
			m_basicStatsWin.AddChild(m_invRenderer);
			m_invRenderer.SetChoice(Direction::Up);
			m_invRenderer.SetFilter(InventoryFilter::All);
			m_invRenderer.ShowItemList();
			AudioMgr::PlaySoundEffect("swish1.ogg", 80);
		}

		if (InputMgr::Instance().WasPressed(VK_UP))
		{
			m_invRenderer.SetChoice(Direction::Up);
			m_invRenderer.ShowItemStats();
		}
		if (InputMgr::Instance().WasPressed(VK_DOWN))
		{
			m_invRenderer.SetChoice(Direction::Down);
			m_invRenderer.ShowItemStats();
		}
		if (InputMgr::Instance().WasPressed(VK_LEFT))
		{
			m_invRenderer.SetChoice(Direction::Left);
			m_invRenderer.ShowItemStats();
		}
		if (InputMgr::Instance().WasPressed(VK_RIGHT))
		{
			m_invRenderer.SetChoice(Direction::Right);
			m_invRenderer.ShowItemStats();
		}
	}
}

///////////////////////
// GWGBattlefieldHUD //
///////////////////////

GWGBattlefieldHUD::GWGBattlefieldHUD(Actor* pActor) : 
	m_landEffectWin(64, 40, Vector2f(8, 8)),
	m_selfStatsWin(120, 40, Vector2f(GAME_SCREEN_WIDTH - 120 - 8, 8)),
	m_pActor(pActor),
	m_statsWinWidth(0)
{
	assert(m_pActor != NULL);
	TileMap& currMap = GameObjectMgr::Instance().GetTileMap();

	int le = currMap.GetLandEffectOfActor(m_pActor);

	std::stringstream temp;
	temp << "LAND\n EFFECT\n   ";
	if (le == 0)
		temp << " ";
	temp << le << "%";

	m_landEffectWin.SetFont(FontMgr::Instance().GetFont(FontStyle::FixedWidth));
	m_landEffectWin.SetText(temp.str().c_str());
	m_landEffectWin.SetSlideDest(CompassDir::W);
	m_landEffectWin.GetSlider().SnapOffScreen();

	m_statsWinWidth = m_pActor->GetStatBlock().GetBFHUDWidth();
	m_selfStatsWin.SetSize((m_statsWinWidth * 8) + 16, 40);
	// right edge minus ((winsizeInLetters + l and r edges) * lettersize) - 8)
	m_selfStatsWin.GetSlider().SetOnScreenPos
		(Vector2f((GAME_SCREEN_WIDTH - ((m_statsWinWidth + 2) * 8) - 8), 8));
	m_selfStatsWin.SetFont(FontMgr::Instance().GetFont(FontStyle::FixedWidth));
	m_selfStatsWin.SetText(m_pActor->GetStatBlock().GetBattlefieldHUDString());
	m_selfStatsWin.SetSlideDest(CompassDir::E);
	m_selfStatsWin.GetSlider().SnapOffScreen();

	AddChild(m_landEffectWin);
	AddChild(m_selfStatsWin);
}

void GWGBattlefieldHUD::SummonWindows()
{
	m_landEffectWin.GetSlider().SlideOntoScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 0);
	m_selfStatsWin.GetSlider().SlideOntoScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 80);
}

void GWGBattlefieldHUD::DismissWindows()
{
	m_landEffectWin.GetSlider().SlideOffScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 0);
	m_selfStatsWin.GetSlider().SlideOffScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 80);
	m_bExpired = true;
}

void GWGBattlefieldHUD::DismissWindowsSilently()
{
	m_landEffectWin.GetSlider().SnapOffScreen();
	m_selfStatsWin.GetSlider().SnapOffScreen();
	m_bExpired = true;
}

bool GWGBattlefieldHUD::IsExpired()
{
	if (m_bExpired && m_landEffectWin.GetSlider().IsOffScreen() && 
		m_selfStatsWin.GetSlider().IsOffScreen())
		return true;
	else
		return false;
}

void GWGBattlefieldHUD::DoUpdate(const float ticks)
{
	TileMap& currMap = GameObjectMgr::Instance().GetTileMap();
	int le = currMap.GetLandEffectOfActor(m_pActor);

	std::stringstream temp;
	temp << "LAND\n EFFECT\n   ";
	if (le == 0)
		temp << " ";
	temp << le << "%";
	m_landEffectWin.SetText(temp.str().c_str());
}

//////////////////////////
// GWGBattleCutsceneHUD //
//////////////////////////

GWGBattleCutsceneHUD::GWGBattleCutsceneHUD(Actor* pPlayer, Actor* pEnemy) : 
	m_playerWin(80, 40, Vector2f(200, 8)),
	m_enemyWin(80, 40, Vector2f(8, 170)),
	m_pPlayer(pPlayer),
	m_pEnemy(pEnemy),
	m_playerWinWidth(0),
	m_enemyWinWidth(0)
{
	assert((m_pPlayer != NULL) && (m_pEnemy != NULL));
	m_playerWinWidth = m_pPlayer->GetStatBlock().GetBFHUDWidth();
	m_playerWin.SetSize((m_playerWinWidth * 8) + 16, 40);
	// right edge minus ((winsizeInLetters + l and r edges) * lettersize) - 8)
	m_playerWin.GetSlider().SetOnScreenPos
		(Vector2f((GAME_SCREEN_WIDTH - ((m_playerWinWidth + 2) * 8) - 8), 8));
	m_playerWin.SetFont(FontMgr::Instance().GetFont(FontStyle::FixedWidth));
	m_playerWin.SetText(m_pPlayer->GetStatBlock().GetBattlefieldHUDString());
	m_playerWin.SetSlideDest(CompassDir::E);
	m_playerWin.GetSlider().SnapOffScreen();

	m_enemyWinWidth = m_pEnemy->GetStatBlock().GetBFHUDWidth();
	m_enemyWin.SetSize((m_enemyWinWidth * 8) + 16, 40);
	m_enemyWin.SetFont(FontMgr::Instance().GetFont(FontStyle::FixedWidth));
	m_enemyWin.SetText(m_pEnemy->GetStatBlock().GetBattlefieldHUDString());
	m_enemyWin.SetSlideDest(CompassDir::W);
	m_enemyWin.GetSlider().SnapOffScreen();

	AddChild(m_playerWin);
	AddChild(m_enemyWin);
}

// these windows are part of the cutscene and do not slide in or out
void GWGBattleCutsceneHUD::SummonWindows()
{
	m_playerWin.GetSlider().SnapOntoScreen();
	m_enemyWin.GetSlider().SnapOntoScreen();
}

void GWGBattleCutsceneHUD::DismissWindows()
{
	m_playerWin.GetSlider().SnapOffScreen();
	m_enemyWin.GetSlider().SnapOffScreen();
	m_bExpired = true;
}

bool GWGBattleCutsceneHUD::IsExpired()
{
	if (m_bExpired && m_playerWin.GetSlider().IsOffScreen() && 
		m_enemyWin.GetSlider().IsOffScreen())
		return true;
	else
		return false;
}

void GWGBattleCutsceneHUD::DoUpdate(const float ticks)
{

}

/////////////////////
// GWGTalkingHeads //
/////////////////////

GWGTalkingHeads::GWGTalkingHeads(Actor* pActor, const std::string& text) : 
	m_faceWin(64, 80, Vector2f(8, 8)),
	m_pActor(pActor)
{
	assert(m_pActor != NULL);

	m_diaWin.SetText(text);
	m_diaWin.SetSlideDest(CompassDir::S);
	m_diaWin.GetSlider().SnapOffScreen();

	m_faceWin.SetDecal(m_pActor->GetFaceTex());
	m_faceWin.SetSlideDest(CompassDir::N);
	m_faceWin.GetSlider().SnapOffScreen();

	AddChild(m_diaWin);
	AddChild(m_faceWin);
}

void GWGTalkingHeads::SummonWindows()
{
	m_diaWin.GetSlider().SlideOntoScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 0);
	m_faceWin.GetSlider().SlideOntoScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 80);
}

void GWGTalkingHeads::DismissWindows()
{
	m_diaWin.GetSlider().SlideOffScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 0);
	m_faceWin.GetSlider().SlideOffScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 80);
	m_bExpired = true;
}

bool GWGTalkingHeads::IsExpired()
{
	if (m_bExpired && m_diaWin.GetSlider().IsOffScreen() && 
		m_faceWin.GetSlider().IsOffScreen())
		return true;
	else
		return false;
}

void GWGTalkingHeads::ProcessInput()
{
	if (InputMgr::Instance().WasPressed(GEN_A) && m_diaWin.AtEndOfString())
		DismissWindows();

	m_diaWin.ProcessInput();
}

/////////////////
// GWGDialogue //
/////////////////

GWGDialogue::GWGDialogue(const std::string& text) 
{
	m_diaWin.SetText(text);
	m_diaWin.SetSlideDest(CompassDir::S);
	m_diaWin.GetSlider().SnapOffScreen();
	AddChild(m_diaWin);
}

void GWGDialogue::SummonWindows()
{
	m_diaWin.GetSlider().SlideOntoScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 0);
}

void GWGDialogue::DismissWindows()
{
	m_diaWin.GetSlider().SlideOffScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 0);
	m_bExpired = true;
}

bool GWGDialogue::IsExpired()
{
	if (m_bExpired && m_diaWin.GetSlider().IsOffScreen())
		return true;
	else
		return false;
}

void GWGDialogue::ProcessInput()
{
	if (InputMgr::Instance().WasPressed(GEN_A) && m_diaWin.AtEndOfString())
		DismissWindows();

	m_diaWin.ProcessInput();
}

////////////////////
// GWGAutoAdvance //
////////////////////

GWGAutoAdvance::GWGAutoAdvance(const std::string& text) 
{
	m_autoWin.SetText(text);
	m_autoWin.SetSlideDest(CompassDir::S);
	m_autoWin.GetSlider().SnapOntoScreen();
	AddChild(m_autoWin);
}

void GWGAutoAdvance::SummonWindows()
{
	m_autoWin.GetSlider().SnapOntoScreen();
}

void GWGAutoAdvance::DismissWindows()
{
	m_autoWin.GetSlider().SnapOffScreen();
	m_bExpired = true;
}

bool GWGAutoAdvance::IsExpired()
{
	if (m_bExpired)
		return true;
	else
		return false;
}

void GWGAutoAdvance::ProcessInput()
{
	m_autoWin.ProcessInput();
}

//////////////////////
// GWGShopping		//
//////////////////////

GWGShopping::GWGShopping() : 
	m_merchandiseWin(216, 48, Vector2f(8, 8)),
	m_itemNameWin(80, 40, Vector2f(0, 44)),
	m_goldWin(72, 32, Vector2f(GAME_SCREEN_WIDTH - 80, GAME_SCREEN_HEIGHT - 40))
{
	m_merchandiseWin.SetFont(FontMgr::Instance().GetFont(FontStyle::FixedWidth));
	m_merchandiseWin.SetText("MERCHANDISE GOES HERE");
	m_merchandiseWin.SetSlideDest(CompassDir::N);
	m_merchandiseWin.GetSlider().SnapOffScreen();

	m_itemNameWin.SetFont(FontMgr::Instance().GetFont(FontStyle::FixedWidth));
	m_itemNameWin.SetText("SHORT\n SWORD\n     140");
	m_itemNameWin.SetSlideDest(CompassDir::W);
	m_itemNameWin.GetSlider().SnapOffScreen();

	m_goldWin.SetFont(FontMgr::Instance().GetFont(FontStyle::FixedWidth));
	m_goldWin.SetText("GOLD\n      0");
	m_goldWin.SetSlideDest(CompassDir::E);
	m_goldWin.GetSlider().SnapOffScreen();

	AddChild(m_merchandiseWin);
	AddChild(m_itemNameWin);
	AddChild(m_goldWin);
}

void GWGShopping::SummonWindows()
{
	m_merchandiseWin.GetSlider().SlideOntoScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 0);
	m_itemNameWin.GetSlider().SlideOntoScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 80);
	m_goldWin.GetSlider().SlideOntoScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 160);
}

void GWGShopping::DismissWindows()
{
	m_merchandiseWin.GetSlider().SlideOffScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 0);
	m_itemNameWin.GetSlider().SlideOffScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 80);
	m_goldWin.GetSlider().SlideOffScreen();
	AudioMgr::PlaySoundEffect("swish1.ogg", 160);
	m_bExpired = true;
}

bool GWGShopping::IsExpired()
{
	if (m_bExpired && m_merchandiseWin.GetSlider().IsOffScreen() &&
		m_itemNameWin.GetSlider().IsOffScreen() && m_goldWin.GetSlider().IsOffScreen())
		return true;
	else
		return false;
}

void GWGShopping::ProcessInput()
{
	if (InputMgr::Instance().WasPressed(GEN_A)) 
	{
		if (!m_bExpired)
		{
			DismissWindows();
		}
	}
}