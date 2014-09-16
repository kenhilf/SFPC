#include "battlecutscene.h"
#include "filepathmgr.h"
#include "gameobjectmgr.h"
#include "actor.h"
#include "statblock.h"

BattleCutscene::BattleCutscene(Actor* pPlayer, Actor* pEnemy) :
	GameObject(LayerSpace::Foreground),
	m_pPlayer(pPlayer),
	m_pEnemy(pEnemy),
	m_pEnemyFrames(NULL),
	m_pPlayerFrames(NULL),
	m_currPlayerPose(BattleAniFrames::Idle),
	m_currEnemyPose(BattleAniFrames::Idle)
{
	assert((m_pPlayer != NULL) && (m_pEnemy != NULL));

	Image buffer;
	std::string imagepath = FilePathMgr::Instance().GetImagePath();

	buffer.LoadBMP(imagepath + "grass.bmp");
	m_footing.LoadImage(buffer);

	buffer.LoadBMP(imagepath + "battlebg01.bmp");
	m_background.LoadImage(buffer);

	buffer.LoadBMP(imagepath + "black.bmp");
	m_black.LoadImage(buffer);

	m_pPlayerFrames = new Tileset(m_pPlayer->GetBattleset(), 5, 1, 128);
	m_playerAni[BattleAniFrames::Idle].SetTileset(m_pPlayerFrames);
	m_playerAni[BattleAniFrames::Attack].SetTileset(m_pPlayerFrames);
	m_playerAni[BattleAniFrames::Evade].SetTileset(m_pPlayerFrames);
	m_playerAni[BattleAniFrames::Idle].AddFrame(0);
	m_playerAni[BattleAniFrames::Idle].AddFrame(1);
	m_playerAni[BattleAniFrames::Idle].GetAnimationIndexer().Init(2, 333, AniType::Loop);	
	m_playerAni[BattleAniFrames::Evade].AddFrame(2);
	m_playerAni[BattleAniFrames::Evade].GetAnimationIndexer().Init(1, 333, AniType::Sequence);	
	m_playerAni[BattleAniFrames::Attack].AddFrame(3);
	m_playerAni[BattleAniFrames::Attack].AddFrame(4);
	m_playerAni[BattleAniFrames::Attack].GetAnimationIndexer().Init(2, 667, AniType::Sequence);	

	m_pEnemyFrames = new Tileset(m_pEnemy->GetBattleset(), 5, 1, 128);
	m_enemyAni[BattleAniFrames::Idle].SetTileset(m_pEnemyFrames);
	m_enemyAni[BattleAniFrames::Attack].SetTileset(m_pEnemyFrames);
	m_enemyAni[BattleAniFrames::Evade].SetTileset(m_pEnemyFrames);
	m_enemyAni[BattleAniFrames::Idle].AddFrame(0);
	m_enemyAni[BattleAniFrames::Idle].AddFrame(1);
	m_enemyAni[BattleAniFrames::Idle].GetAnimationIndexer().Init(2, 266, AniType::Loop);
	m_enemyAni[BattleAniFrames::Evade].AddFrame(2);
	m_enemyAni[BattleAniFrames::Evade].GetAnimationIndexer().Init(1, 333, AniType::Sequence);	
	m_enemyAni[BattleAniFrames::Attack].AddFrame(3);
	m_enemyAni[BattleAniFrames::Attack].AddFrame(4);
	m_enemyAni[BattleAniFrames::Attack].GetAnimationIndexer().Init(2, 667, AniType::Sequence);	

	GameObjectMgr::Instance().AddObject(m_inputBlocker);
	GameObjectMgr::Instance().FocusOnObject(&m_inputBlocker);
}

BattleCutscene::~BattleCutscene()
{
	GameObjectMgr::Instance().PopFocusObject();
	GameObjectMgr::Instance().RemoveObject(m_inputBlocker);

	if (m_pEnemyFrames != NULL)
		delete m_pEnemyFrames;

	if (m_pPlayerFrames != NULL)
		delete m_pPlayerFrames;
}

void BattleCutscene::DoUpdate(const float ticks)
{
	m_playerAni[m_currPlayerPose].DoUpdate(ticks);
	m_enemyAni[m_currEnemyPose].DoUpdate(ticks);
}

void BattleCutscene::DoDraw(DrawMgr& drawMgr)
{
	drawMgr.DrawQuad(m_black, Vector2f(0.0f, 0.0f), DrawSpace::Screen, FlipMode::None, ScaleMode::Direct, (float)GAME_SCREEN_WIDTH, (float)GAME_SCREEN_HEIGHT);
	drawMgr.DrawQuad(m_background, Vector2f(0.0f, 60.0f), DrawSpace::Screen, FlipMode::None, ScaleMode::Percentage, 1.25f, 1.25f);
	drawMgr.DrawQuad(m_footing, Vector2f(185.0f, 167.0f), DrawSpace::Screen, FlipMode::None, ScaleMode::None);

	if (m_pEnemy->GetStatBlock().IsAlive())
		drawMgr.DrawQuad(m_enemyAni[m_currEnemyPose].GetActiveFrame(), Vector2f(40.0f, 40.0f), DrawSpace::Screen, FlipMode::None, ScaleMode::None);
	if (m_pPlayer->GetStatBlock().IsAlive())
		drawMgr.DrawQuad(m_playerAni[m_currPlayerPose].GetActiveFrame(), Vector2f(162.0f, 55.0f), DrawSpace::Screen, FlipMode::None, ScaleMode::None);
}

void BattleCutscene::SetPlayerPose(const BattleAniFrames::Type pose)
{
	m_currPlayerPose = pose;
	m_playerAni[m_currPlayerPose].Reset();
}

void BattleCutscene::SetEnemyPose(const BattleAniFrames::Type pose)
{
	m_currEnemyPose = pose;
	m_enemyAni[m_currEnemyPose].Reset();
}