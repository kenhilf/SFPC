#ifndef _gameobjectmgr_h_
#define _gameobjectmgr_h_

#include "gameobject.h"
#include "camera.h"
#include "tilemap.h"
#include "tilemapeditor.h"
#include "actor.h"
#include "drawmgr.h"
#include "gamewindow.h"

typedef std::vector<GameObject*> GameObjectList;
typedef std::vector<GameWindow*> GameWindowList;

class GameObjectMgr
{
public:
	static GameObjectMgr& Instance() { static GameObjectMgr instance; return instance; }
	~GameObjectMgr();

	void LoadMap(const std::string& mapname);

	void AddObject(GameObject& obj);
	bool RemoveObject(GameObject& obj);
	void AddObject(Actor& actor);
	void RemoveActor(Actor& actor);
	void AddObject(GameWindow& gw);
	void AddObject(TileMap& map);
	void AddObject(TileMapEditor& editor);
	void AddObject(Camera& cam);

	void Update(const float ticks); 
	void Physics(const float ticks); 
	void Draw(DrawMgr& drawMgr);

	Camera& GetCamera();
	TileMap& GetTileMap();
	TileMapEditor& GetMapEditor();
	ActorList& GetActorList();
	GameWindowList& GetGameWindowList();

	void FocusOnObject(GameObject* obj);
	bool CheckCurrentFocus(GameObject* p_obj);
	bool HasFocusObject();
	bool PopFocusObject();
	void ProcessFocusInput();

	void QuitGame();
	bool GetExitState();

private:
	// hide constructor for singleton pattern
	GameObjectMgr();

	// disable copy constructor and =operator
	GameObjectMgr(const GameObjectMgr& rhs);
	GameObjectMgr& operator=(const GameObjectMgr& rhs);

	ActorList m_actorList;
	GameObjectList m_objListByLayer[LayerSpace::NumTypes];
	GameWindowList m_gameWindowList;
	GameObjectList m_focusObjList;

	TileMap* m_pTileMap;
	TileMapEditor* m_pTileMapEditor;
	Camera* m_pCamera;
	bool m_bReadyToExit;
};

#endif //_gameobjectmgr_h_

