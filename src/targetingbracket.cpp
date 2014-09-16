#include "targetingbracket.h"
#include "groundbattle.h"
#include "audiomgr.h"
#include "actor.h"
#include "inputmgr.h"

TargetingBracket::TargetingBracket() :
	GameObject(LayerSpace::Overlay),
	m_bracket("sfbrackets.bmp", 8, 1)
{
	
}

TargetingBracket::~TargetingBracket()
{

}

void TargetingBracket::DoUpdate(const float ticks)
{
	SetWorldPos(GroundBattle::Instance().GetCurrTarget().GetWorldPos());
}

void TargetingBracket::DoDraw(DrawMgr& drawMgr)
{
	drawMgr.DrawQuad(m_bracket.GetTile(0), GetWorldPos(), DrawSpace::Camera);
}

void TargetingBracket::ProcessInput()
{
	if (InputMgr::Instance().WasPressed(VK_LEFT) ||
		InputMgr::Instance().WasPressed(VK_DOWN)) 
	{
		AudioMgr::PlaySoundEffect("swish1.ogg");
		GroundBattle::Instance().PrevTarget();
	}

	if (InputMgr::Instance().WasPressed(VK_UP) ||
		InputMgr::Instance().WasPressed(VK_RIGHT)) 
	{
		AudioMgr::PlaySoundEffect("swish1.ogg");
		GroundBattle::Instance().NextTarget();
	}

	if (InputMgr::Instance().WasPressed(GEN_A))
	{
		AudioMgr::PlaySoundEffect("ping.ogg");
		GroundBattle::Instance().Attack();
	}

	if (InputMgr::Instance().WasPressed(GEN_B))
		GroundBattle::Instance().ShowMoveRange();
}