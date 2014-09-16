#ifndef _tilemap_h_
#define _tilemap_h_

#include "core.h"
#include "tilemaplayer.h"
#include "tileset.h"
#include "gameobject.h"
#include "actor.h"
#include "gametimer.h"
#include "array2d.h"
class Line;

class TileMap : public GameObject
{
public:
	TileMap(const std::string& filename);
	virtual ~TileMap();
	TileMapLayer& GetLayer(const int layer);
	Tileset* GetTerrainTiles();
	Tileset* GetUtilityTiles();
	int GetNumLayers() const;

	int GetWidthInTiles() const;
	int GetWidthInPixels() const;
	int GetHeightInTiles() const;
	int GetHeightInPixels() const;

	void SetTileSet(const int layer, Tileset* const ts);
	void SetPos(const Vector2f& pos);

	void TileFill(const int tilenum, const int layer, 
				  const int x1, const int y1, const int x2, const int y2);

	bool IsPosSolid(const Position& pos) const;
//	bool IsPosSolid(const Vector2f& pos, const Vector2f& vel);
	void SetPosSolid(const Position& pos);

	Actor* GetActorAtPos(const Position& pos) const;
	Actor* GetActorAtCoord(const Vector2f& vec);
	void SetActorAtPos(const Position& pos, Actor* pActor);
	void SetActorAtCoord(const Vector2f& vec, Actor* pActor);

	Terrain::Type GetTerrainAtPos(const Position& pos) const;
	void SetTerrainAtPos(const Position& pos, const Terrain::Type terr);
	int GetLandEffectOfActor(Actor* pActor);

	bool CanActorEnter(Actor* pActor, const Position& pos) const;
	bool CanActorStay(Actor* pActor, const Position& pos) const;
	float GetCostToEnter(Actor* pActor, const Position& pos) const;

	void SetEventAtPos(const Position& pos, const int eventNum);
	int GetEventAtPos(const Position& pos) const;
	int GetNumEvents() const;
	int AddNewEvent();

	void ShowSolidMarkers();
	void HideSolidMarkers();
	void ToggleSolidMarkers();

	void ShowTerrainMarkers();
	void HideTerrainMarkers();
	void ToggleTerrainMarkers();

	void ShowEventMarkers();
	void HideEventMarkers();
	void ToggleEventMarkers();

	void ClearFlashMarkers(const bool flash = false);
	void ShowFlashMarkers();
	void HideFlashMarkers();
	void SetPosFlashing(const Position& pos);
	bool IsPosFlashing(const Position& pos);
	void CalculateWalkable(int x, int y, float unitsleft, Actor* pActor);

	ActorList RangeCheck(Actor& currActor);
	void MarkDiagonalLine(Line& l, const ActorTeam::Type opponent, ActorList& aList);

	void DrawLayer(DrawMgr &drawMgr, const int layer);
	virtual void DoUpdate(const float ticks);

	void SetDebugTint(const Position& pos, const Pixel32& color);
	void ClearDebugTint();

	void SaveToDisk();
	void LoadFromDisk(const std::string& filename);
	static void CreateNewMap(const std::string& name, const int width, const int height);

private:
	void ClampPos();

	std::string m_mapName;

	TileMapLayer m_layer[3];
	int m_numLayers;
	
	int m_numEvents;

	Vector2f m_pos;

	int m_widthInTiles;
	int m_heightInTiles;

	struct TileInfo
	{
		TileInfo() : isSolid(false), pActor(NULL), ground(Terrain::Smooth), isFlashing(true), eventNum(0) {}
		bool isSolid;
		Actor* pActor;
		Terrain::Type ground;
		bool isFlashing;
		int eventNum;
	};

	Array2d<TileInfo, int> m_tileInfo;
	Array2d<Pixel32, int> m_debugTint;

	Tileset m_backgroundTiles;
	Tileset m_overlayTiles;
	Tileset m_foregroundTiles;
	Tileset m_utilityTiles;
	Tileset m_terrainTiles;

	float m_flashPct;
	GameTimer::Tracker m_flashTracker;
	bool m_bIncreaseFlash;

	bool m_bShowSolids;
	bool m_bShowWalkable;
	bool m_bShowTerrain;
	bool m_bShowEvents;
	bool m_bShowPathing;
};

#endif //_tilemap_h_