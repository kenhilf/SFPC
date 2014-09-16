#include "gameengine.h"
#include "gameobjectmgr.h"
#include "inputmgr.h"
#include "tilemap.h"
#include "menumgr.h"
#include "statblock.h"
#include "intro.h"
#include "outro.h"
#include "windowmgr.h"
#include "commandmgr.h"
#include "filepathmgr.h"
#include "audiomgr.h"
#include "gamemodemgr.h"
#include "actormgr.h"
#include "groundbattle.h"
#include "fpscounter.h"

GameEngine::GameEngine() : 
	m_bIsActive(true),
	m_bQuit(false),
	m_pos(0, 0),
	m_timer(TimerMode::SysClock),
	m_frameTicks(0),
	m_pActor(NULL),
	m_elapsedTime(0)
{

}

void GameEngine::Initialize()
{
	// Create the OpenGL Window.  Actual window size defined in core.h
	bluGL::Instance().CreateGLWindow("Shining Force PC", ACTUAL_SCREEN_WIDTH, ACTUAL_SCREEN_HEIGHT, 32, false);

	// Register all keys to be used
	InputMgr::Instance().RegisterKey(VK_ESCAPE);
	InputMgr::Instance().RegisterKey(VK_F1);
	InputMgr::Instance().RegisterKey(VK_F2);
	InputMgr::Instance().RegisterKey(VK_F3);
	InputMgr::Instance().RegisterKey(VK_F5);
	InputMgr::Instance().RegisterKey(VK_F6);
	InputMgr::Instance().RegisterKey(VK_F7);
	InputMgr::Instance().RegisterKey(VK_F8);
	InputMgr::Instance().RegisterKey(VK_F9);
	InputMgr::Instance().RegisterKey(VK_F11);
	InputMgr::Instance().RegisterKey(VK_F12);
	InputMgr::Instance().RegisterKey(VK_HOME);
	InputMgr::Instance().RegisterKey(VK_END);
	InputMgr::Instance().RegisterKey(VK_PRIOR);
	InputMgr::Instance().RegisterKey(VK_NEXT);

	InputMgr::Instance().RegisterKey(VK_SPACE);

	InputMgr::Instance().RegisterKey('W');
	InputMgr::Instance().RegisterKey('A');
	InputMgr::Instance().RegisterKey('S');
	InputMgr::Instance().RegisterKey('D');
	InputMgr::Instance().RegisterKey('Z');
	InputMgr::Instance().RegisterKey('X');
	InputMgr::Instance().RegisterKey('C');
	InputMgr::Instance().RegisterKey('Q');
	InputMgr::Instance().RegisterKey('E');
	InputMgr::Instance().RegisterKey('R');
	InputMgr::Instance().RegisterKey('F');

	// Initial camera setup
	static Camera camera(0, 0, 0.25f);
	GameObjectMgr::Instance().AddObject(camera);

	// Initialize widely used resources
	FontMgr::Instance().Init();
	GameWindowTexMgr::Instance().Init();
	ActionIconMgr::Instance().Init();
	AudioMgr::Init();
	GameModeMgr::Init();
	CommandMgr::Instance().LoadItemStartupScript();
	GameObjectMgr::Instance().LoadMap("camp");
	CommandMgr::Instance().LoadGameStartupScript();
	
	// Set up map editor
	static TileMapEditor tme;
	GameObjectMgr::Instance().AddObject(tme);

	// Final camera setup
	m_pActor = &ActorMgr::Instance().GetActivePlayer();

	camera.SnapToActor(m_pActor);
	m_drawMgr.SetCamera(camera);
	GameObjectMgr::Instance().FocusOnObject(&camera);

//	static FPSCounter fps;
//	GameObjectMgr::Instance().AddObject(fps);

	// Run the intro!
//	static GameIntro intro;
//	GameObjectMgr::Instance().AddObject(intro);
//	GameObjectMgr::Instance().FocusOnObject(&intro);
}

void GameEngine::ProcessInput()
{
	Camera& camera = GameObjectMgr::Instance().GetCamera();
	Actor& activePlayer = ActorMgr::Instance().GetActivePlayer();
	TileMap& currMap = GameObjectMgr::Instance().GetTileMap();
	TileMapEditor& editor = GameObjectMgr::Instance().GetMapEditor();

	// First process engine keys that always apply
	// Esc to quit
	if (InputMgr::Instance().WasReleased(VK_ESCAPE))	
	{
		m_bQuit = true;
//		CommandMgr::Instance().AddCommandManually(new GCShowOutroAndQuit());
	}

	// Home to toggle between field mode and map editing mode
	if (InputMgr::Instance().WasPressed(VK_HOME))
	{
		GameMode::Type gamemode = GameModeMgr::GetMode();
		if (gamemode == GameMode::Field)
		{
			GameModeMgr::SetMode(GameMode::EditMap);
			editor.EditMap(&currMap);
		}
		else if (gamemode == GameMode::EditMap)
		{
			GameModeMgr::SetMode(GameMode::Field);
			editor.ReleaseMap();
			camera.FollowNewActor(&activePlayer);
			m_pActor = &activePlayer;
		}
		else if (gamemode == GameMode::Battle)
		{
			EndBattle();
		}
	}

	// End to enter battle mode
/*	if (InputMgr::Instance().WasPressed(VK_END))
	{
		StartBattle();
	}
*/
	// Process Field mode specific input
	GameMode::Type gamemode = GameModeMgr::GetMode();
	if (gamemode == GameMode::Field)
	{
		if (InputMgr::Instance().WasReleased(VK_F1))
		{
			camera.FollowNewActor(&ActorMgr::Instance().GetNextPlayer());
		}
	}

	// Process Map Editing specific input
	if (gamemode == GameMode::EditMap)
	{
		editor.ProcessInput();
	}

	// Process Battle specific input
	if (gamemode == GameMode::Battle)
	{
		m_pActor = camera.GetCurrActor();
		Position pos = m_pActor->GetLocalPos().CoordToTileNum();

		if ((InputMgr::Instance().WasReleased(GEN_A)) &&
	 		!(MenuMgr::Instance().HasActiveMenus()) &&
			!(GameObjectMgr::Instance().HasFocusObject()) &&
			!(GroundBattle::Instance().IsCutsceneRunning()) &&
			!(m_pActor->IsMoving()) && 
			!(camera.IsMoving()) &&
			!(m_pActor->GetTeam() == ActorTeam::Enemy))
		{
			// default choice is stay
			int choice = 2;

			// but if any targets are in range, default becomes Attack
			if (GroundBattle::Instance().SilentTargetCheck(m_pActor))
				choice = 0;

			// confirm the current actor is not trying to end turn in an ally's square
 			if ((currMap.GetActorAtPos(pos) == m_pActor) ||	(currMap.GetActorAtPos(pos) == NULL))
				MenuMgr::Instance().AddFourDirMenu(ActionName::Attack, ActionName::Item, 
												   ActionName::Stay, ActionName::Magic, choice);
			else
				AudioMgr::PlaySoundEffect("error.ogg");
		}
	}

	// finally process input on whatever has focus regardless of mode
	// typically Camera sits at the bottom of the pile for character movement purposes.
 	GameObjectMgr::Instance().ProcessFocusInput();
}

bool GameEngine::GameLoop()
{
	if (m_bIsActive)
	{
		InputMgr::Instance().Update();
		ProcessInput();
	}

	MenuMgr::Instance().ProcessResults();

//	m_bQuit = GameObjectMgr::Instance().GetExitState();
	if(m_bQuit)
	{
		return false;
	}

	// clear the screen
	m_drawMgr.BeginFrame();

	// get frame ticks and pass this along to update all gameobjects
	m_frameTicks = m_timer.GetFrameTicks();

	// manual map update for flashing tiles to display properly
	GameObjectMgr::Instance().GetTileMap().Update(m_frameTicks);
	GameObjectMgr::Instance().Update(m_frameTicks);
	CommandMgr::Instance().Update(m_frameTicks);
	AudioMgr::Update(m_frameTicks);
	GroundBattle::Instance().Update(m_frameTicks);

	// Clean up any menus ready for deletion
	MenuMgr::Instance().Update();
	WindowMgr::Instance().Update(m_frameTicks);

	// Update collision of gameobjects
	GameObjectMgr::Instance().Physics(m_frameTicks);

	// update the camera/map relationship
	m_pos = GameObjectMgr::Instance().GetCamera().GetPos();
	GameObjectMgr::Instance().GetTileMap().SetPos(m_pos);

	// draw all gameobjects
	GameObjectMgr::Instance().Draw(m_drawMgr);

	// flip buffers
	m_drawMgr.EndFrame();

	return true;
}

void GameEngine::StartBattle()
{
	Camera& camera = GameObjectMgr::Instance().GetCamera();
	TileMap& currMap = GameObjectMgr::Instance().GetTileMap();

	GameMode::Type gamemode = GameModeMgr::GetMode();
	if (gamemode == GameMode::EditMap)
	{
		GameObjectMgr::Instance().GetMapEditor().ReleaseMap();
	}
	if ((gamemode == GameMode::Field) || (gamemode == GameMode::EditMap))
	{
		GameModeMgr::SetMode(GameMode::Battle);
		GroundBattle::Instance().StartBattle();
		m_pActor = &GroundBattle::Instance().NextTurn();
		camera.FollowNewActor(m_pActor);
		currMap.ClearFlashMarkers();
		Position pos = m_pActor->GetLocalPos().CoordToTileNum();
		currMap.CalculateWalkable(pos.x, pos.y, static_cast<float>(m_pActor->GetStatBlock().GetMovement()+1), m_pActor);
	}
}

void GameEngine::EndBattle()
{
	Camera& camera = GameObjectMgr::Instance().GetCamera();
	Actor& activePlayer = ActorMgr::Instance().GetActivePlayer();
	TileMap& currMap = GameObjectMgr::Instance().GetTileMap();

	GroundBattle::Instance().EndBattle();
	GameModeMgr::SetMode(GameMode::Field);
	camera.FollowNewActor(&activePlayer);
	m_pActor = &activePlayer;
	currMap.ClearFlashMarkers(true);
}

void GameEngine::SetActive(const bool active)
{
	m_bIsActive = active;
}

bool GameEngine::IsActive() const
{
	return m_bIsActive;
}

void GameEngine::Shutdown()
{
	bluGL::Instance().KillGLWindow();
	AudioMgr::Shutdown();
}
