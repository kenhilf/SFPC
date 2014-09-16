#include "gameobjectmgr.h"
#include "actormgr.h"
#include "stlex.h"

GameObjectMgr::GameObjectMgr() :
	m_pTileMap(NULL),
	m_pTileMapEditor(NULL),
	m_pCamera(NULL),
	m_bReadyToExit(false)
{

}

GameObjectMgr::~GameObjectMgr()
{
	if (m_pTileMap != NULL)
	{
		delete m_pTileMap;
	}
}

void GameObjectMgr::LoadMap(const std::string& mapname)
{
	if (m_pTileMap != NULL)
	{
		// remove all the actors from the current map.
		// 6/8 pm - I think the while loop here may not be the most efficient thing, but it
		// works, no more double time after the second warp.
		//@TODO: decide if there's a better way of doing this
		int i = m_actorList.size() - 1;
		while (m_actorList.size() > 0)
		{
			RemoveActor(*m_actorList[i]);
			--i;
		}

		// clear enemies and NPCs from the ActorMgr
		ActorMgr::Instance().ClearNPCs();
		ActorMgr::Instance().ClearEnemies();

		delete m_pTileMap;
	}

	m_pTileMap = new TileMap(mapname);

	if (ActorMgr::Instance().HasActivePlayer())
	{
		AddObject(ActorMgr::Instance().GetActivePlayer());
		m_pCamera->FollowNewActor(&ActorMgr::Instance().GetActivePlayer());
	}
}

void GameObjectMgr::AddObject(GameObject& obj)
{
	m_objListByLayer[obj.GetLayerSpace()].push_back(&obj);
}

bool GameObjectMgr::RemoveObject(GameObject& obj)
{
	bool result = false;
	for (int i = 0; i < LayerSpace::NumTypes; i++)
	{
		if (EraseFromSTLContainer(m_objListByLayer[i], &obj))
			result = true;
	}
	return result;
}

void GameObjectMgr::AddObject(Actor& actor)
{
	AddObject(static_cast<GameObject&>(actor));
	m_actorList.push_back(&actor);
}

void GameObjectMgr::RemoveActor(Actor& actor)
{
	EraseFromSTLContainer(m_actorList, &actor);
	EraseFromSTLContainer(m_objListByLayer[actor.GetLayerSpace()], &actor);
}

void GameObjectMgr::AddObject(GameWindow& gw)
{
	AddObject(static_cast<GameObject&>(gw));
	m_gameWindowList.push_back(&gw);
}

void GameObjectMgr::AddObject(TileMap& map)
{
	AddObject(static_cast<GameObject&>(map));
	m_pTileMap = &map;
}

void GameObjectMgr::AddObject(TileMapEditor& editor)
{
	AddObject(static_cast<GameObject&>(editor));
	m_pTileMapEditor = &editor;
}

void GameObjectMgr::AddObject(Camera& cam)
{
	AddObject(static_cast<GameObject&>(cam));
	m_pCamera = &cam;
}

void GameObjectMgr::Update(const float ticks)
{
	for (int i = 0; i < LayerSpace::NumTypes; i++)
	{
		GameObjectList::iterator iter = m_objListByLayer[i].begin();
		for ( ; iter != m_objListByLayer[i].end(); ++iter)
		{
			(*iter)->Update(ticks);
		}
	}
}

void GameObjectMgr::Physics(const float ticks)
{
	for (int i = 0; i < LayerSpace::NumTypes; i++)
	{
		GameObjectList::iterator iter = m_objListByLayer[i].begin();
		for ( ; iter != m_objListByLayer[i].end(); ++iter)
		{
			(*iter)->Physics(ticks);
		}
	}
}

void GameObjectMgr::Draw(DrawMgr& drawMgr)
{
	for (int i = 0; i < LayerSpace::UpdateOnly; i++)
	{
		// If the map has a presence at this layer, draw that first
		if (m_pTileMap != NULL)
		{
			if (i < m_pTileMap->GetNumLayers())
			{
				m_pTileMap->DrawLayer(drawMgr, i);
			}
		}
		// then draw anything in that layer on top of the map
		GameObjectList::iterator iter = m_objListByLayer[i].begin();
		for ( ; iter != m_objListByLayer[i].end(); ++iter)
		{
			(*iter)->Draw(drawMgr);
		}

		//@HACK: draw the active player again so it is on top when passing through
		// ally occupied squares
		if (i == static_cast<int>(LayerSpace::Overlay))
			m_pCamera->GetCurrActor()->DoDraw(drawMgr);
	}
}

Camera& GameObjectMgr::GetCamera()
{
	assert(m_pCamera);
	return *m_pCamera;
}

TileMap& GameObjectMgr::GetTileMap()
{
	assert(m_pTileMap);
	return *m_pTileMap;
}

TileMapEditor& GameObjectMgr::GetMapEditor()
{
	assert(m_pTileMapEditor);
	return *m_pTileMapEditor;
}

ActorList& GameObjectMgr::GetActorList()
{
	return m_actorList;
}

GameWindowList& GameObjectMgr::GetGameWindowList()
{
	return m_gameWindowList;
}

void GameObjectMgr::FocusOnObject(GameObject* obj)
{
 	m_focusObjList.push_back(obj);
}

bool GameObjectMgr::CheckCurrentFocus(GameObject* p_obj)
{
	if (p_obj == m_focusObjList.back())
		return true;
	else
		return false;
}

bool GameObjectMgr::HasFocusObject()
{
	// camera is always at the bottom, so > 1, not > 0
	if (m_focusObjList.size() > 1)
		return true;
	else
		return false;
}

bool GameObjectMgr::PopFocusObject()
{
	if ((m_focusObjList.size() > 1) && (m_focusObjList.front() == m_pCamera))
	{
		m_focusObjList.pop_back();
		return true;
	}
	else
		return false;
}

void GameObjectMgr::ProcessFocusInput()
{
	m_focusObjList.back()->ProcessInput();
}

void GameObjectMgr::QuitGame()
{
	m_bReadyToExit = true;
}

bool GameObjectMgr::GetExitState()
{
	return m_bReadyToExit;
}
