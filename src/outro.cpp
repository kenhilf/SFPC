#include "outro.h"
#include "inputmgr.h"
#include "gameobjectmgr.h"

GameOutro::GameOutro()
{
	m_logo = m_black;
	m_fadePct = 0.0f;
	m_fadeTracker.SetRate(50);

	m_openingText.SetOffset(0, 4);
	m_openingText.SetString("\n            Shining Force PC\n \n \n  A Remake of the Classic Strategy RPG\n         Programmed By Ken Hilf\n \n \n Uses Assets from Shining Force by Sega\n     Shining Force I  (C) Sega 1992\n     Shining Force II (C) Sega 1994\n     Shining Force CD (C) Sega 1994\n \n     Uses the NeHe OpenGL Base Code\n            nehe.gamedev.net\n \n     Uses the Audiere Sound Library\n         audiere.sourceforge.net\n \n \n          - Special Thanks To -\n \n     Tony Maiorano, Nathan Yam, and\n  everyone in FireBell over the years\n          for all your support!\n \n \n           Thanks for playing!");
}

GameOutro::~GameOutro()
{

}

void GameOutro::ProcessInput()
{
	if (InputMgr::Instance().WasPressed(GEN_A) || 
		InputMgr::Instance().WasPressed(GEN_B) || 
		InputMgr::Instance().WasPressed(GEN_C) || 
		InputMgr::Instance().WasPressed(VK_ESCAPE)) 
	{
		GameObjectMgr::Instance().QuitGame();
	}
}