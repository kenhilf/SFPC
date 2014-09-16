#include "tilemapeditor.h"
#include <sstream>
#include <fstream>
#include "gameobjectmgr.h"
#include "commandmgr.h"
#include "filepathmgr.h"
#include "inputmgr.h"

TileMapEditor::TileMapEditor() :
	GameObject(LayerSpace::Window),
	m_editMode(EditorMode::Tiles),
	m_pCurrMap(NULL),
	m_cursor(ActorParams("CURSOR", "Flying", "sfbrackets.bmp", 9, 9)),
	m_currLayer(0),
	m_currTile(0),
	m_currEvent(0),
	m_brushSize(1),
	m_currEventName("No Event")
{
	m_cursor.SetRate(0.192f);
	m_cursor.IgnoreSolids();
	m_cursor.SetTeam(ActorTeam::Editor);
}

TileMapEditor::~TileMapEditor()
{

}

void TileMapEditor::EditMap(TileMap* pMap)
{
	m_pCurrMap = pMap;
	m_pCurrMap->ClearFlashMarkers(true);
	m_pCurrMap->ShowSolidMarkers();
	m_currLayer = 0;
	m_editMode = EditorMode::Tiles;
	ClearBrush();
	AddChild(m_hud);
	GameObjectMgr::Instance().AddObject(m_cursor);
	GameObjectMgr::Instance().GetCamera().FollowNewActor(&m_cursor);
}

void TileMapEditor::ReleaseMap()
{
	RemoveChild(m_hud);
	GameObjectMgr::Instance().RemoveActor(m_cursor);
	m_pCurrMap->HideSolidMarkers();
	m_pCurrMap->ClearFlashMarkers(true);
	m_pCurrMap->HideTerrainMarkers();
	m_pCurrMap->HideEventMarkers();
	m_pCurrMap = NULL;
}

void TileMapEditor::DoUpdate(const float ticks)
{
	Position pos = CurrTilePos();
	std::stringstream temp;
	temp << "EditMode: ";
	if (m_editMode == EditorMode::Tiles)
		temp << "Tiles\nCurrTile:      " << m_currTile
			 << "\nLayer: " << m_currLayer 
			 << "\nBrush Size: " << m_brushSize;
	else if (m_editMode == EditorMode::Terrain)
		temp << "Terrain\nCurrTile: \n";
	else // m_editMode == EditorMode::Events
		temp << "Events\nEventNum: " << m_currEvent << "\nEventString: " << m_currEventName;
	m_hud.SetString(temp.str());
}

void TileMapEditor::DoDraw(DrawMgr& drawMgr)
{
	if (m_pCurrMap != NULL)
	{
		Tileset* pTileset = NULL;
		if (m_editMode == EditorMode::Tiles)
		{
			pTileset = m_pCurrMap->GetLayer(m_currLayer).GetTileSet();
			drawMgr.DrawQuad(pTileset->GetTile(m_currTile), Vector2f(58.0f, 13.0f), DrawSpace::Screen, FlipMode::None, ScaleMode::Percentage, 0.75f, 0.75f);

			drawMgr.GlobalBlend(0.5f);
			for (int j = 0; j < m_brushSize; ++j)
			{
				for (int i = 0; i < m_brushSize; ++i)
				{
					Vector2f pos = m_cursor.GetWorldPos();
					int tile = m_brush(i, j);
					drawMgr.DrawQuad(pTileset->GetTile(m_brush(i, j)), 
									 Vector2f(pos.x + (i * TILE_WIDTH), pos.y + (j * TILE_HEIGHT)),
									 DrawSpace::Camera);
				}
			}
			drawMgr.ResetRGBA();
		}
		if (m_editMode == EditorMode::Terrain)
		{
			pTileset = m_pCurrMap->GetUtilityTiles();
			drawMgr.DrawQuad(pTileset->GetTile(2), Vector2f(58.0f, 13.0f), DrawSpace::Screen, FlipMode::None, ScaleMode::Percentage, 0.75f, 0.75f);
			pTileset = m_pCurrMap->GetTerrainTiles();
			drawMgr.DrawQuad(pTileset->GetTile(m_currTile), Vector2f(58.0f, 13.0f), DrawSpace::Screen, FlipMode::None, ScaleMode::Percentage, 0.75f, 0.75f);
		}
	}
}

void TileMapEditor::ProcessInput()
{
	if (InputMgr::Instance().WasPressed('W')) 
	{
		SwitchTile(Direction::Up);
	}
	if (InputMgr::Instance().WasPressed('S')) 
	{
		SwitchTile(Direction::Down);
	}
	if (InputMgr::Instance().WasPressed('A')) 
	{
		SwitchTile(Direction::Left);
	}
	if (InputMgr::Instance().WasPressed('D')) 
	{
		SwitchTile(Direction::Right);
	}

	if (InputMgr::Instance().WasPressed('Z') && (m_editMode == EditorMode::Tiles)) 
	{
		SwitchLayer();
	}
	if (InputMgr::Instance().WasPressed('X')) 
	{
		ToggleSolid();
	}
	if (InputMgr::Instance().WasPressed('C')) 
	{
		CopyTile();
	}
	if (InputMgr::Instance().WasPressed('Q')) 
	{
		if (m_editMode == EditorMode::Tiles)
		{
			m_pCurrMap->ToggleSolidMarkers();
		}
		else if (m_editMode == EditorMode::Terrain)
		{
			m_pCurrMap->ToggleSolidMarkers();
			m_pCurrMap->ToggleTerrainMarkers();
		}
		else //m_editMode == EditorMode::Events
		{
			m_pCurrMap->ToggleSolidMarkers();
			m_pCurrMap->ToggleEventMarkers();
		}
	}
	if (InputMgr::Instance().WasPressed('E')) 
	{
		ChangeEditMode();
	}
	if (InputMgr::Instance().WasPressed('R') && (m_editMode == EditorMode::Events)) 
	{
		AddNewEvent();
	}
	if (InputMgr::Instance().WasPressed('F') && (m_editMode == EditorMode::Events)) 
	{
		EditEventScript(m_currEvent);
	}
	
	if (InputMgr::Instance().IsDown(GEN_A) && (m_editMode != EditorMode::Events)) 
	{
		ApplyTile();
	}
	if (InputMgr::Instance().WasReleased(GEN_A) && (m_editMode == EditorMode::Events)) 
	{
		ApplyTile();
	}

	if (InputMgr::Instance().WasPressed(VK_F5)) 
	{
		m_pCurrMap->SaveToDisk();
	}

	if (InputMgr::Instance().WasPressed(VK_F1)) 
	{
		OpenMapDialogue();
	}

	if (InputMgr::Instance().WasPressed(VK_F9)) 
	{
		ExecuteConsoleScript();
	}

	if (InputMgr::Instance().WasPressed(VK_F12)) 
	{
		OpenConsoleScript();
	}

	if (InputMgr::Instance().WasPressed(VK_PRIOR)) 
	{
		GrowBrush();
	}

	if (InputMgr::Instance().WasPressed(VK_NEXT)) 
	{
		ShrinkBrush();
	}
}

void TileMapEditor::ChangeEditMode()
{
	ClearBrush();
	m_pCurrMap->ShowSolidMarkers();
	if (m_editMode == EditorMode::Tiles)
	{
		m_pCurrMap->ShowTerrainMarkers();
		m_editMode = EditorMode::Terrain;
		return;
	}
	else if (m_editMode == EditorMode::Terrain)
	{
		m_pCurrMap->HideTerrainMarkers();
		m_pCurrMap->ShowEventMarkers();
		m_editMode = EditorMode::Events;
		return;
	}
	else if (m_editMode == EditorMode::Events)
	{
		m_pCurrMap->HideEventMarkers();
		m_editMode = EditorMode::Tiles;
		return;
	}
}

void TileMapEditor::SwitchLayer()
{
	m_currLayer++;
	m_currLayer = m_currLayer % m_pCurrMap->GetNumLayers();
	ClearBrush();
}

void TileMapEditor::ToggleSolid()
{
	m_pCurrMap->SetPosSolid(CurrTilePos());
}

void TileMapEditor::SwitchTile(const Direction::Type dir)
{
	if (m_editMode == EditorMode::Events)
	{
		switch(dir)
		{
			case Direction::Up:
				m_currEvent++;
				break;
			case Direction::Down:
				m_currEvent--;
				break;
			case Direction::Left:
				m_currEvent--;
				break;
			case Direction::Right:
				m_currEvent++;
				break;
		}

		int size = m_pCurrMap->GetNumEvents();
		if (m_currEvent > size)
			m_currEvent = 0;
		if (m_currEvent < 0)
			m_currEvent = size;
		
		m_currEventName = CommandMgr::Instance().GetEventScriptName(m_currEvent);

		return;
	}

	Tileset* pTiles = NULL;
	if (m_editMode == EditorMode::Tiles)
		pTiles = m_pCurrMap->GetLayer(m_currLayer).GetTileSet();
	else if (m_editMode == EditorMode::Terrain)
		pTiles = m_pCurrMap->GetTerrainTiles();

	int rowsize = pTiles->GetRowWidth();
	int numtiles = pTiles->GetNumTiles();

	switch(dir)
	{
		case Direction::Up:
			m_currTile -= rowsize;
			break;
		case Direction::Down:
			m_currTile += rowsize;
			break;
		case Direction::Left:
			m_currTile--;
			break;
		case Direction::Right:
			m_currTile++;
			break;
	}

	if (m_currTile < 0)
		m_currTile += numtiles;

	if (m_currTile >= numtiles)
		m_currTile -= numtiles;

	if (m_editMode == EditorMode::Tiles)
		FillBrush(m_currTile);
}

void TileMapEditor::ApplyTile()
{
	Position pos = CurrTilePos();
	if (m_editMode == EditorMode::Tiles)
	{
		for (int j = 0; j < m_brushSize; ++j)
		{
			for (int i = 0; i < m_brushSize; ++i)
			{
				m_pCurrMap->GetLayer(m_currLayer).SetTile(m_brush(i, j), pos.x + i, pos.y + j);
			}
		}
	}
	else if (m_editMode == EditorMode::Terrain)
		m_pCurrMap->SetTerrainAtPos(pos, static_cast<Terrain::Type>(m_currTile));
	else if (m_editMode == EditorMode::Events)
		m_pCurrMap->SetEventAtPos(pos, m_currEvent);
}

void TileMapEditor::CopyTile()
{
	Position pos = CurrTilePos();
	if (m_editMode == EditorMode::Tiles)
	{
		m_currTile = m_pCurrMap->GetLayer(m_currLayer).GetTile(pos.x, pos.y);
		for (int j = 0; j < m_brushSize; ++j)
		{
			for (int i = 0; i < m_brushSize; ++i)
			{
				m_brush(i, j) = m_pCurrMap->GetLayer(m_currLayer).GetTile(pos.x + i, pos.y + j);
			}
		}
	}		
	else if (m_editMode == EditorMode::Terrain)
		m_currTile = static_cast<int>(m_pCurrMap->GetTerrainAtPos(pos));
	else if (m_editMode == EditorMode::Events)
	{
		m_currEvent = m_pCurrMap->GetEventAtPos(pos);
		m_currEventName = CommandMgr::Instance().GetEventScriptName(m_currEvent);
	}
}

void TileMapEditor::EditEventScript(const int scriptnum)
{
	if (scriptnum < 1)
		return;

	std::stringstream filename;
	filename << "e";
	if (scriptnum < 100)
		filename << "0";
	if (scriptnum < 10)
		filename << "0";
	filename << scriptnum << ".sfs";

	std::string scriptFilename = FilePathMgr::Instance().GetMapScriptPath() + filename.str();

	std::ifstream infile;
	infile.open(scriptFilename.c_str());
	if (!(infile.is_open()))
	{
		std::ofstream outfile;
		outfile.open(scriptFilename.c_str());
		outfile << "!SFPC_Script \"Event ";
		if (scriptnum < 100)
			outfile << "0";
		if (scriptnum < 10)
			outfile << "0";
		outfile << scriptnum << "\"\n\n";
		outfile.close();
	}
	infile.close();

	ShellExecute(NULL, NULL, "notepad.exe", scriptFilename.c_str(), NULL, SW_SHOW);
}

void TileMapEditor::AddNewEvent()
{
	m_currEvent = m_pCurrMap->AddNewEvent();
	EditEventScript(m_currEvent);
	m_currEventName = CommandMgr::Instance().GetEventScriptName(m_currEvent);
}

Position TileMapEditor::CurrTilePos()
{
	Position pos = m_cursor.GetWorldPos().CoordToTileNum();
	return pos;
}

//@TODO: this is still very much a work in progress!
void TileMapEditor::OpenMapDialogue()
{
//	char pathbuff[MAX_PATH];
	char filebuff[MAX_PATH];
	std::string mapname;
//	std::string path;

//	GetModuleFileName(NULL, pathbuff, 256);
//	path = pathbuff;
//	path = path.substr(0, path.rfind("\\"));
//	path += "\\game\\data\\maps\\";
//	path += MAP_PATH;

	OPENFILENAME opf;
	ZeroMemory(&opf, sizeof(opf));
	opf.hwndOwner = 0;
	opf.lpstrFilter = "SFPC Map\0*.sfm\0";
	opf.lpstrCustomFilter = 0;
	opf.nMaxCustFilter = 0L;
	opf.nFilterIndex = 1L;
	opf.lpstrFile = filebuff;
	opf.lpstrFile[0] = '\0';
	opf.nMaxFile = 256;
	opf.lpstrFileTitle = 0;
	opf.nMaxFileTitle = 50;
	opf.lpstrInitialDir = FilePathMgr::Instance().GetMapPath().c_str();
	opf.lpstrTitle = "Open Map...";
	opf.nFileOffset = 0;
	opf.nFileExtension = 0;
	opf.lpstrDefExt = "*.*";
	opf.lpfnHook = NULL;
	opf.lCustData = 0;
	opf.Flags = (OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT) & ~OFN_ALLOWMULTISELECT;
	opf.lStructSize = sizeof(OPENFILENAME);

	if (GetOpenFileName(&opf))
	{
		mapname = opf.lpstrFile;
		int i = mapname.rfind("\\");
		mapname = mapname.substr(i + 1);
		mapname = mapname.substr(0, mapname.length() - 4);
		ReleaseMap();
		GameObjectMgr::Instance().LoadMap(mapname);
		EditMap(&(GameObjectMgr::Instance().GetTileMap()));
	}
}

void TileMapEditor::OpenConsoleScript()
{
	std::string scriptFilename = FilePathMgr::Instance().GetScriptPath() + "console.sfs";

	std::ofstream outfile;
	outfile.open(scriptFilename.c_str());
	outfile << "!SFPC_Script \"Console Command\"\n\n";
	outfile.close();

	ShellExecute(NULL, NULL, "notepad.exe", scriptFilename.c_str(), NULL, SW_SHOW);
}

void TileMapEditor::ExecuteConsoleScript()
{
	CommandMgr::Instance().LoadConsoleScript();
}

void TileMapEditor::GrowBrush()
{
	if (m_brushSize < 5)
	{
		++m_brushSize;
		m_brush.Reset(m_brushSize, m_brushSize);
		FillBrush(m_currTile);
	}
}

void TileMapEditor::ShrinkBrush()
{
	if (m_brushSize > 1)
	{
		--m_brushSize;
		m_brush.Reset(m_brushSize, m_brushSize);
		FillBrush(m_currTile);
	}
}

void TileMapEditor::ClearBrush()
{
	m_currTile = 0;
	m_brushSize = 1;
	m_brush.Reset(m_brushSize, m_brushSize);
	m_brush(0, 0) = 0;
}

void TileMapEditor::FillBrush(const int val)
{
	for (int j = 0; j < m_brushSize; ++j)
	{
		for (int i = 0; i < m_brushSize; ++i)
		{
			m_brush(i, j) = val;
		}
	}
}
