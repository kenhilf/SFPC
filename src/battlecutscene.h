#ifndef _battlecutscene_h_
#define _battlecutscene_h_

#include "gameobject.h"
#include "texture.h"
#include "animation.h"
class Actor;

namespace BattleAniFrames
{
	enum Type { Idle, Attack, Evade, NumTypes };
}

class BattleCutscene : public GameObject
{
public:
	BattleCutscene(Actor* pPlayer, Actor* pEnemy);
	virtual ~BattleCutscene();
	virtual void DoUpdate(const float ticks);
	virtual void DoDraw(DrawMgr& drawMgr);
	void SetPlayerPose(const BattleAniFrames::Type pose);
	void SetEnemyPose(const BattleAniFrames::Type pose);

private:
	Texture m_footing;
	Texture m_background;
	Texture m_black;
	
	Actor* m_pPlayer;
	Actor* m_pEnemy;

	Tileset* m_pEnemyFrames;
	Tileset* m_pPlayerFrames;

	Animation m_playerAni[BattleAniFrames::NumTypes];
	Animation m_enemyAni[BattleAniFrames::NumTypes];

	BattleAniFrames::Type m_currPlayerPose;
	BattleAniFrames::Type m_currEnemyPose;

	GameObject m_inputBlocker;
};

#endif //_battlecutscene_h_