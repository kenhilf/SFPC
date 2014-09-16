#include "tilemap.h"
#include <iostream>
#include <fstream>
#include <assert.h>
#include "inputmgr.h"
#include "gameobjectmgr.h"
#include "filepathmgr.h"
#include "commandmgr.h"
#include "line.h"
#include "statblock.h"
#include "blumath.h"
#include "line.h"

//////////////////////
// TileMap			//
//////////////////////

TileMap::TileMap(const std::string& filename) :
	GameObject(LayerSpace::UpdateOnly),
	m_mapName(filename),
	m_numLayers(3),
	m_numEvents(0),
	m_pos(0, 0),
//	m_widthInTiles(TILEMAP_WIDTH),
//	m_heightInTiles(TILEMAP_HEIGHT),
	m_widthInTiles(0),
	m_heightInTiles(0),
	m_utilityTiles("utilitytiles24_5x1.bmp", 4, 1),
	m_terrainTiles("terraintiles24_6x1.bmp", 6, 1),
	m_flashPct(0.0f),
	m_flashTracker(250),
	m_bIncreaseFlash(false),
	m_bShowSolids(false),
	m_bShowWalkable(false),
	m_bShowTerrain(false),
	m_bShowEvents(false),
	m_bShowPathing(true)
{
	FilePathMgr::Instance().SetCurrMapName(filename);

	//@TODO: should a layer hold and be in charge of its own tileset?
	//@HACK: the tileset pointers are initially null before the LoadFromDisk goes off, but 
	// the first thing that happens there is the tilesets get loaded and they are no longer 
	// null by the time they are first accessed.  Need to consider how bad this is, if at all.
	m_layer[0].SetTileSet(&m_backgroundTiles);
	m_layer[1].SetTileSet(&m_overlayTiles);
	m_layer[2].SetTileSet(&m_foregroundTiles);

	LoadFromDisk(filename);

	// load the map startup script
	CommandMgr::Instance().LoadMapScript(m_mapName);
}

TileMap::~TileMap()
{

}

TileMapLayer& TileMap::GetLayer(const int layer)
{
	assert((layer < m_numLayers) && !(layer < 0));
	return m_layer[layer];
}

Tileset* TileMap::GetTerrainTiles()
{
	return &m_terrainTiles;
}

Tileset* TileMap::GetUtilityTiles()
{
	return &m_utilityTiles;
}

int TileMap::GetNumLayers() const
{
	return m_numLayers;
}

int TileMap::GetWidthInTiles() const
{
	return m_widthInTiles;
}

int TileMap::GetWidthInPixels() const
{
	return m_widthInTiles * TILE_WIDTH;
}

int TileMap::GetHeightInTiles() const
{
	return m_heightInTiles;
}

int TileMap::GetHeightInPixels() const
{
	return m_heightInTiles * TILE_HEIGHT;
}

void TileMap::SetTileSet(const int layer, Tileset* const ts)
{
	assert((layer < m_numLayers) && !(layer < 0));
	m_layer[layer].SetTileSet(ts);
}

void TileMap::SetPos(const Vector2f& pos)
{
	m_pos = pos;
	ClampPos();
}

void TileMap::TileFill(const int tilenum, const int layer, const int x1, const int y1, const int x2, const int y2)
{
	// This command should only run in bounds on valid rectangles, otherwise it will do nothing.
	// Don't want to assert or crash on a scripting typo and risk losing unsaved mapping work.
	if ((layer < 0) || (layer >= 3) || (x2 < x1) || (y2 < y1) || 
		(x1 < 0)  || (x2 < 0) || (y1 < 0) || (y2 < 0) ||
		(x1 >= m_widthInTiles) || (x2 >= m_widthInTiles) || 
		(y1 >= m_heightInTiles) || (y2 >= m_heightInTiles) )
		return;

	for (int y = y1; y <= y2; ++y)
	{
		for (int x = x1; x <= x2; ++x)
		{
			m_layer[layer].SetTile(tilenum, x, y);
		}
	}
}

bool TileMap::IsPosSolid(const Position& pos) const
{
	//@TEMP: while out of bounds is not "solid" per se, treat it as if it were and don't
	// let an actor go out of bounds.  
	if ((pos.x < 0) || (pos.x >= m_widthInTiles) ||
		(pos.y < 0) || (pos.y >= m_heightInTiles) )
		return true;

	return m_tileInfo(pos.x, pos.y).isSolid;
}

void TileMap::SetPosSolid(const Position& pos)
{
	m_tileInfo(pos.x, pos.y).isSolid = !m_tileInfo(pos.x, pos.y).isSolid;
}

Actor* TileMap::GetActorAtPos(const Position& pos) const
{
	//@TODO: behaves correctly, but fails silently.  need a cleaner way to do this.
	if ((pos.x >= m_widthInTiles) || (pos.x < 0))
		return NULL;

	if ((pos.y >= m_heightInTiles) || (pos.y < 0))
		return NULL;

	return m_tileInfo(pos.x, pos.y).pActor;
}

Actor* TileMap::GetActorAtCoord(const Vector2f& vec)
{
	Position p = vec.CoordToTileNum();
	return GetActorAtPos(Position(p.x, p.y));
}

void TileMap::SetActorAtPos(const Position& pos, Actor* pActor)
{
	m_tileInfo(pos.x, pos.y).pActor = pActor;
}

void TileMap::SetActorAtCoord(const Vector2f& vec, Actor* pActor)
{
	Position p = vec.CoordToTileNum();
	m_tileInfo(p.x, p.y).pActor = pActor;
}

Terrain::Type TileMap::GetTerrainAtPos(const Position& pos) const
{
	return m_tileInfo(pos.x, pos.y).ground;
}

void TileMap::SetTerrainAtPos(const Position& pos, const Terrain::Type terr)
{
	m_tileInfo(pos.x, pos.y).ground = terr;
}

int TileMap::GetLandEffectOfActor(Actor* pActor)
{
	Position pos = pActor->GetLocalPos().CoordToTileNum();
	float le = pActor->GetStatBlock().GetLandEffect(m_tileInfo(pos.x, pos.y).ground) * 100;
	return static_cast<int>(le);
}

bool TileMap::CanActorEnter(Actor* pActor, const Position& pos) const
{
	if (IsPosSolid(pos))
		return false;

	//@TODO: need to get rid of that hardcoded 20 for impassible by terrain
	if (GetCostToEnter(pActor, pos) >= 20.0f)
		return false;

	Actor* destActor = GetActorAtPos(pos);
	if (destActor != NULL)
	{
//		if (destActor->GetTeam() != pActor->GetTeam())
			return false;
	}

	return true;
}

bool TileMap::CanActorStay(Actor* pActor, const Position& pos) const
{
	if (IsPosSolid(pos))
		return false;

	Actor* destActor = GetActorAtPos(pos);
	if (destActor != NULL)
	{
		return false;
	}

	return true;
}

float TileMap::GetCostToEnter(Actor* pActor, const Position& pos) const
{
	return pActor->GetStatBlock().GetLandEffect(m_tileInfo(pos.x, pos.y).ground) + 1.0f;
}

void TileMap::SetEventAtPos(const Position& pos, const int eventNum)
{
	m_tileInfo(pos.x, pos.y).eventNum = eventNum;
}

int TileMap::GetEventAtPos(const Position& pos) const
{
	return m_tileInfo(pos.x, pos.y).eventNum;
}

int TileMap::GetNumEvents() const
{
	return m_numEvents;
}

int TileMap::AddNewEvent()
{
	assert(m_numEvents < 999);
	++m_numEvents;
	return m_numEvents;
}

void TileMap::ShowSolidMarkers()
{
	m_bShowSolids = true;
}

void TileMap::HideSolidMarkers()
{
	m_bShowSolids = false;
}

void TileMap::ToggleSolidMarkers()
{
	m_bShowSolids = !m_bShowSolids;
}

void TileMap::ShowTerrainMarkers()
{
	m_bShowTerrain = true;
}

void TileMap::HideTerrainMarkers()
{
	m_bShowTerrain = false;
}

void TileMap::ToggleTerrainMarkers()
{
	m_bShowTerrain = !m_bShowTerrain;
}

void TileMap::ShowEventMarkers()
{
	m_bShowEvents = true;
}

void TileMap::HideEventMarkers()
{
	m_bShowEvents = false;
}

void TileMap::ToggleEventMarkers()
{
	m_bShowEvents = !m_bShowEvents;
}

void TileMap::ClearFlashMarkers(const bool flash)
{
	for (int j = 0; j < m_heightInTiles; j++)
	{
		for (int i = 0; i < m_widthInTiles; i++)
		{
			m_tileInfo(i, j).isFlashing = flash;
		}
	}
	if (flash) // flashing mode has been turned off, no more flashing
		HideFlashMarkers();
}

void TileMap::ShowFlashMarkers()
{
	m_bShowWalkable = true;
}

void TileMap::HideFlashMarkers()
{
	m_bShowWalkable = false;
}

void TileMap::SetPosFlashing(const Position& pos)
{
	m_tileInfo(pos.x, pos.y).isFlashing = true; 
}

bool TileMap::IsPosFlashing(const Position& pos) 
{
	// TEMP: while out of bounds is not "walkable" per se, we don't want actors going out
	// of bounds, so say no if asked if out of bounds is walkable/flashing.
	if ((pos.x < 0) || (pos.x >= m_widthInTiles) ||
		(pos.y < 0) || (pos.y >= m_heightInTiles) )
		return false;

	return m_tileInfo(pos.x, pos.y).isFlashing; 
}

void TileMap::CalculateWalkable(int x, int y, float unitsleft, Actor* pActor)
{
	ShowFlashMarkers();
	Position pos = Position(x, y);
	float stepcost = 1.0f + pActor->GetStatBlock().GetLandEffect(m_tileInfo(x, y).ground);

	// if this tile is solid, return
	if (IsPosSolid(pos))
		return;

	// if there is an enemy at the specified position, return
	Actor* pDestActor = GetActorAtPos(pos);
	if (pDestActor != NULL)
	{
		if (pDestActor->GetTeam() != pActor->GetTeam())
			return;
	}

	if (unitsleft >= stepcost) // if we have enough movement left to make a move
	{
		unitsleft -= stepcost; // subtract the cost of a move through this tile
		SetPosFlashing(pos);   // flag this position as walkable

		// and try walking to each of the remaining adjacent tiles with whatever 
		// movement we have left
		CalculateWalkable(x, y-1, unitsleft, pActor);
		CalculateWalkable(x, y+1, unitsleft, pActor);
		CalculateWalkable(x-1, y, unitsleft, pActor);
		CalculateWalkable(x+1, y, unitsleft, pActor);
	}
	else // unitsleft < stepcost, we don't have enough movement left to make a move
		return;
}

ActorList TileMap::RangeCheck(Actor& currActor)
{
	ActorTeam::Type foe = ActorTeam::Opponent(currActor.GetTeam());
	Position curr = currActor.GetWorldPos().CoordToTileNum();
	ActorList targets;
	Position pos = currActor.GetWorldPos().CoordToTileNum();
	int x = pos.x;
	int y = pos.y;
	int range = 0;
	std::vector<int> rangeList = currActor.GetStatBlock().GetAttackRange();
	ClearFlashMarkers();

	// go through each range and mark it.
	// look at each marked square, if there's an actor there, check it's team
	// if it's an opponent of the current actor, put it on the return list
	for (unsigned int i = 0; i < rangeList.size(); i++)
	{
		range = rangeList[i];
		if (range == 0)
			SetPosFlashing(pos);
		else
		{
			Line line = Line(x+range, y, x, y+range);
			MarkDiagonalLine(line, foe, targets);
			line = Line(x, y+range, x-range, y);
			MarkDiagonalLine(line, foe, targets);
			line = Line(x-range, y, x, y-range);
			MarkDiagonalLine(line, foe, targets);
			line = Line(x, y-range, x+range, y);
			MarkDiagonalLine(line, foe, targets);
		}
	}

	return targets;
}

void TileMap::MarkDiagonalLine(Line& l, const ActorTeam::Type opponent, ActorList& aList)
{
	int x = 0;
	int y = 0;
	Position pos = Position(0, 0);
	Actor* pTarget = NULL;
	while (l.GetNextPoint(x, y))
	{
		pos = Position(x, y);
		if (!IsPosSolid(pos))
		{
			SetPosFlashing(pos);
			pTarget = GetActorAtPos(pos);
			if (pTarget != NULL)
			{
				if (pTarget->GetTeam() == opponent)
					aList.push_back(pTarget);
				pTarget = NULL;
			}
		}
	}
}

void TileMap::DrawLayer(DrawMgr &drawMgr, const int layer)
{
	Tileset* currTileset = m_layer[layer].GetTileSet();
	int w = currTileset->GetTileDataWidth();
	int h = currTileset->GetTileDataHeight();

	Position tilepos = m_pos.CoordToTileNum();

	for (int j = tilepos.y; j < (tilepos.y + GAME_SCREEN_HEIGHT_IN_TILES + 1); j++)
	{
		for (int i = tilepos.x; i < (tilepos.x + GAME_SCREEN_WIDTH_IN_TILES + 1); i++)
		{
			int tilenum = m_layer[layer].GetTile(i, j);
			float destx = (i*w) - m_pos.x;
			float desty = (j*h) - m_pos.y;

			if (layer == 0)
			{
				drawMgr.DrawQuad(currTileset->GetTile(tilenum), Vector2f(destx, desty), DrawSpace::Screen);
			}
			if (layer == 1)
			{
				if (tilenum != 0)
					drawMgr.DrawQuad(currTileset->GetTile(tilenum), Vector2f(destx, desty), DrawSpace::Screen);

				if (m_bShowWalkable && m_tileInfo(i, j).isFlashing)
				{
					drawMgr.OneTimeBlend(m_flashPct);
					drawMgr.DrawQuad(m_utilityTiles.GetTile(1), Vector2f(destx, desty), DrawSpace::Screen);
				}
			}
			if (layer == 2)
			{
				if (tilenum != 0)
					drawMgr.DrawQuad(currTileset->GetTile(tilenum), Vector2f(destx, desty), DrawSpace::Screen);

				if (m_bShowSolids && m_tileInfo(i, j).isSolid)
				{
					drawMgr.DrawQuad(m_utilityTiles.GetTile(0), Vector2f(destx, desty), DrawSpace::Screen);
				}
				if (m_bShowTerrain && !m_tileInfo(i, j).isSolid)
				{
					drawMgr.DrawQuad(m_terrainTiles.GetTile(static_cast<int>(m_tileInfo(i, j).ground)), Vector2f(destx, desty), DrawSpace::Screen);
				}
				if (m_bShowEvents && (m_tileInfo(i, j).eventNum > 0))
				{
					drawMgr.DrawQuad(m_utilityTiles.GetTile(3), Vector2f(destx, desty), DrawSpace::Screen);
				}
				if (m_bShowPathing)  
				{
					Pixel32 color = m_debugTint(i, j);
					if ((color.r != 0) || (color.g != 0) || (color.b != 0))
					{
						drawMgr.GlobalBlend(0.5f);
						drawMgr.GlobalTint(color.r/255.0f, color.g/255.0f, color.b/255.0f); 
						drawMgr.DrawQuad(m_utilityTiles.GetTile(1), Vector2f(destx, desty), DrawSpace::Screen);
						drawMgr.ResetRGBA();
					}
				}
			}
		}
	}
}

void TileMap::DoUpdate(const float ticks)
{
	m_flashTracker += ticks;

	int change = m_flashTracker.GetUnitsAsInt();
	if (change > 0)
	{
		if (m_bIncreaseFlash)
		{
			m_flashPct += (static_cast<float>(change) * 0.01f);
			if (m_flashPct > 0.5f)
			{
				m_flashPct = 0.5f;
				m_bIncreaseFlash = false;
			}
		}
		else // flash is decreasing
		{
			m_flashPct -= (static_cast<float>(change) * 0.01f);
			if (m_flashPct < 0.0f)
			{
				m_flashPct = 0.0f;
				m_bIncreaseFlash = true;
			}
		}
	}
}

void TileMap::SetDebugTint(const Position& pos, const Pixel32& color)
{
	m_debugTint(pos.x, pos.y) = color;
}

void TileMap::ClearDebugTint()
{
	m_debugTint.Reset(m_widthInTiles, m_heightInTiles);
}

void TileMap::ClampPos()
{
	if (m_pos.y < 0.0f)
		m_pos.y = 0.0f; 
	if (m_pos.y > static_cast<float>((m_heightInTiles * TILE_HEIGHT) - GAME_SCREEN_HEIGHT - 1)) 
		m_pos.y = static_cast<float>((m_heightInTiles * TILE_HEIGHT) - GAME_SCREEN_HEIGHT - 1); 
	if (m_pos.x < 0.0f)
		m_pos.x = 0.0f;
	if (m_pos.x > static_cast<float>((m_widthInTiles * TILE_WIDTH) - GAME_SCREEN_WIDTH - TILE_WIDTH - 1)) 
		m_pos.x = static_cast<float>((m_widthInTiles * TILE_WIDTH) - GAME_SCREEN_WIDTH - TILE_WIDTH - 1);
}

void TileMap::SaveToDisk()
{
	std::ofstream outfile;
	//@TODO: need to get windows dialoguebox for saving up and running
	const std::string savefilename = FilePathMgr::Instance().GetMapPath() + m_mapName + "/" 
									 + m_mapName + ".sfm";
	outfile.open(savefilename.c_str(), std::ios::out | std::ios::trunc);
	outfile << "!SFPC_Map_Data_Version " << 3 << "\n";

	outfile << m_backgroundTiles.GetFilename().c_str() << " " 
			<< m_backgroundTiles.GetRowWidth() << " "
			<< m_backgroundTiles.GetNumRows() << "\n";

	outfile << m_overlayTiles.GetFilename().c_str() << " " 
			<< m_overlayTiles.GetRowWidth() << " "
			<< m_overlayTiles.GetNumRows() << "\n";

	outfile << m_foregroundTiles.GetFilename().c_str() << " " 
			<< m_foregroundTiles.GetRowWidth() << " "
			<< m_foregroundTiles.GetNumRows() << "\n";

	outfile << m_numLayers << " " << m_widthInTiles << " " << m_heightInTiles << " " << m_numEvents << "\n";
	for (int j = 0; j < m_heightInTiles; j++)
	{
		for (int i = 0; i < m_widthInTiles; i++)
		{
			for (int k = 0; k < m_numLayers; k++)
			{
				outfile << m_layer[k].GetTile(i, j) << " ";
			}
			outfile << m_tileInfo(i, j).isSolid << " " << static_cast<int>(m_tileInfo(i, j).ground) 
				<< " " << m_tileInfo(i, j).eventNum << "\n";
		}
	}
	outfile.close();
}

//@TODO: v3 of map loads breaks compatibility with old maps, disable 1 and 2 or shore them
// up by loading some default tilesets?  should probably just disable.
void TileMap::LoadFromDisk(const std::string& filename)
{
	std::ifstream infile;
	std::string infilename = FilePathMgr::Instance().GetMapPath() + filename + "/" + filename + ".sfm";
	infile.open(infilename.c_str());
	if (!(infile.is_open()))
		assert(false && "Failed to open specified file.");

	std::string buffer;
	infile >> buffer;
	assert(buffer == "!SFPC_Map_Data_Version");
	int ver;
	infile >> ver;
	int buff = 0;

	std::string tsName;
	std::string temp;
	int tsWidth = 0;
	int tsHeight = 0;

	switch(ver)
	{
		case 1: 
			assert(false && "Cannot load map, incompatible old file format!");
			// currently not supported
/*			infile >> m_numLayers >> m_widthInTiles >> m_heightInTiles;
			for (int j = 0; j < m_heightInTiles; j++)
			{
				for (int i = 0; i < m_widthInTiles; i++)
				{
					for (int k = 0; k < m_numLayers; k++)
					{
						infile >> buff;
						m_layer[k].SetTile(buff, i, j);
					}
					infile >> m_tileInfo[i][j].isSolid >> buff;
					m_tileInfo[i][j].ground = static_cast<Terrain::Type>(buff);
				}
			}
			m_numLayers = 3;
*/			break;
		case 2:
			assert(false && "Cannot load map, incompatible old file format!");
			// currently not supported
/*			infile >> m_numLayers >> m_widthInTiles >> m_heightInTiles >> m_numEvents;
			for (int j = 0; j < m_heightInTiles; j++)
			{
				for (int i = 0; i < m_widthInTiles; i++)
				{
					for (int k = 0; k < m_numLayers; k++)
					{
						infile >> buff;
						m_layer[k].SetTile(buff, i, j);
					}
					infile >> m_tileInfo[i][j].isSolid >> buff;
					m_tileInfo[i][j].ground = static_cast<Terrain::Type>(buff);
					infile >> m_tileInfo[i][j].eventNum;
					// uncomment to wipe old events on load
					// m_tileInfo[i][j].eventNum = 0;
				}
			}
*/			break;
		case 3:
			infile >> tsName >> tsWidth >> tsHeight;
			m_backgroundTiles.LoadTiles(tsName, tsWidth, tsHeight);

			infile >> tsName >> tsWidth >> tsHeight;
			m_overlayTiles.LoadTiles(tsName, tsWidth, tsHeight);

			infile >> tsName >> tsWidth >> tsHeight;
			m_foregroundTiles.LoadTiles(tsName, tsWidth, tsHeight);

			infile >> m_numLayers >> m_widthInTiles >> m_heightInTiles >> m_numEvents;

			// reset Array2d m_tileInfo and each layer of m_layer to the correct size
			m_tileInfo.Reset(m_widthInTiles, m_heightInTiles);
			m_debugTint.Reset(m_widthInTiles, m_heightInTiles);
			for (int l = 0; l < 3; ++l) //@HACK: hard coded to 3 layers
			{
				m_layer[l].Reset(m_widthInTiles, m_heightInTiles);
			}

			for (int j = 0; j < m_heightInTiles; j++)
			{
				for (int i = 0; i < m_widthInTiles; i++)
				{
					for (int k = 0; k < m_numLayers; k++)
					{
						infile >> buff;
						m_layer[k].SetTile(buff, i, j);
					}
					infile >> m_tileInfo(i, j).isSolid >> buff;
					m_tileInfo(i, j).ground = static_cast<Terrain::Type>(buff);
					infile >> m_tileInfo(i, j).eventNum;
					// uncomment to wipe old events on load
					// m_tileInfo(i, j).eventNum = 0;
				}
			}
			break;
		default:
			assert(false && "Error in header of specified map file.");
	}
	infile.close();
}

void TileMap::CreateNewMap(const std::string& name, const int width, const int height)
{
	// test to see if a map exists in the folder for the map we are trying to create
	// if so, the map we are trying to make already exists and we should stop here
	std::ifstream infile;
	std::string infilename = FilePathMgr::Instance().GetMapPath() + name + "/" + name + ".sfm";
	infile.open(infilename.c_str());
	if (infile.is_open())
	{
		infile.close();
		assert(false && "Failed to create specified map, map already exists.");
		return;
	}

	// create the directory /game/data/maps/"name" and then the scripts subdirectory
	std::string directory = FilePathMgr::Instance().GetMapPath() + name;
	CreateDirectory(directory.c_str(), NULL);
	directory += "/scripts";
	CreateDirectory(directory.c_str(), NULL);

	// minimum width of 28 (2 screens wide), max of 256
	int x = width;
	if (x < 28)  x = 28;
	if (x > 256) x = 256;

	// minimum height of 20 (2 screens tall), max of 256
	int y = height;
	if (y < 20)  y = 20;
	if (y > 256) y = 256;

	// write out blank map file header
	std::ofstream outfile;
	const std::string mapname = FilePathMgr::Instance().GetMapPath() + name + "/" + name + ".sfm";
	outfile.open(mapname.c_str(), std::ios::out | std::ios::trunc);
	outfile << "!SFPC_Map_Data_Version " << 3 << "\n";  // map header, version number
	outfile << "tileset24_8x24.bmp 8 24\n";				// background tileset, w, h
	outfile << "overlay24_8x24.bmp 8 24\n";				// overlay tileset, w, h
	outfile << "overlay24_8x24.bmp 8 24\n";				// foreground tileset, w, h
	outfile << 3 << " "									// layers (engine currently supports 3)
			<< x << " "									// map width in tiles 
			<< y << " " 								// map height in tiles
			<< 0 << "\n";								// number of events (starts at 0, obv.)

	// write out the blank .sfm map file data
	// .sfm data row format:
	// BackgroundTileNum OverlayTileNum ForegroundTileNum isSolid LandEffect EventNum
	for (int j = 0; j < y; j++) // height
	{
		for (int i = 0; i < x; i++) // width
		{
			for (int k = 0; k < 3; k++) // layers
			{
				//outfile << m_layer[k].GetTile(i, j) << " ";
				outfile << 0 << " ";
			}
			/*outfile << m_tileInfo[i][j].isSolid << " " 
					<< static_cast<int>(m_tileInfo[i][j].ground) << " "
					<< m_tileInfo[i][j].eventNum << "\n";
			*/
			outfile << 0 << " "   // solid data 
					<< 0 << " "   // land effect data
					<< 0 << "\n"; // eventNum data
		}
	}
	outfile.close();

	// write out blank map startup script to the /mapname/scripts folder
	const std::string mapscriptname = directory + "/m000.sfs";
	outfile.open(mapscriptname.c_str(), std::ios::out | std::ios::trunc);
	outfile << "!SFPC_Script \"" << name << " startup script\"\n\n";
	outfile.close();

	// write out blank actor 0 script to the /mapname/scripts folder
	const std::string actorscriptname = directory + "/a000.sfs";
	outfile.open(actorscriptname.c_str(), std::ios::out | std::ios::trunc);
	outfile << "!SFPC_Script \"Actor 000\"\n\nDialogueWindow \"...\"";
	outfile.close();

}
