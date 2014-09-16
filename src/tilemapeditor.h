#ifndef _tilemapeditor_h_
#define _tilemapeditor_h_

#include "actor.h"
#include "array2d.h"
class TileMap;

namespace EditorMode
{
	enum Type { Tiles, Terrain, Events, NumTypes };
}

class TileMapEditor : public GameObject
{
public:
	TileMapEditor();
	virtual ~TileMapEditor();
	void EditMap(TileMap* pMap);
	void ReleaseMap();
	virtual void DoUpdate(const float ticks);
	virtual void DoDraw(DrawMgr& drawMgr);
	virtual void ProcessInput();

private:
	void ChangeEditMode();
	void SwitchLayer();
	void ToggleSolid();
	void SwitchTile(const Direction::Type dir = Direction::Right);
	void ApplyTile();
	void CopyTile();
	void EditEventScript(const int scriptnum);
	void AddNewEvent();
	Position CurrTilePos();
	void OpenMapDialogue();
	void OpenConsoleScript();
	void ExecuteConsoleScript();
	void GrowBrush();
	void ShrinkBrush();
	void ClearBrush();
	void FillBrush(const int val);

	EditorMode::Type m_editMode;
	TileMap* m_pCurrMap;
	Actor m_cursor;
	GameString m_hud;
	int m_currLayer;
	int m_currTile;
	int m_currEvent;
	int m_brushSize;
	Array2d<int, int> m_brush;
	std::string m_currEventName;
};

#endif //_tilemapeditor_h_