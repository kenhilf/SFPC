#include "filepathmgr.h"
#include <windows.h>

FilePathMgr::~FilePathMgr()
{

}

const std::string& FilePathMgr::GetCurrMapName() const
{
	return m_currMapName;
}

void FilePathMgr::SetCurrMapName(const std::string& mapname)
{
	m_currMapName = mapname;
	m_currMapScriptPath = m_mapPath + mapname + "/scripts/";
}

const std::string& FilePathMgr::GetBasePath() const
{
	return m_basePath;
}

const std::string& FilePathMgr::GetTilePath() const
{
	return m_tilePath;
}

const std::string& FilePathMgr::GetImagePath() const
{
	return m_imagePath;
}

const std::string& FilePathMgr::GetScriptPath() const
{
	return m_scriptPath;
}

const std::string& FilePathMgr::GetMapPath() const
{
	return m_mapPath;
}

const std::string& FilePathMgr::GetMapScriptPath() const
{
	return m_currMapScriptPath;
}

const std::string& FilePathMgr::GetFontPath() const
{
	return m_fontPath;
}

const std::string& FilePathMgr::GetSoundPath() const
{
	return m_soundPath;
}

FilePathMgr::FilePathMgr()
{
	char pathbuff[MAX_PATH];

	GetModuleFileName(NULL, pathbuff, MAX_PATH);
	m_basePath = pathbuff;

	m_basePath = m_basePath.substr(0, m_basePath.rfind("\\"));
	m_tilePath = m_basePath + "/data/images/";
	m_imagePath = m_basePath + "/data/images/";
	m_scriptPath = m_basePath + "/data/scripts/";
	m_mapPath = m_basePath + "/data/maps/";
	m_fontPath = m_basePath + "/data/fonts/";
	m_soundPath = m_basePath + "/data/sounds/";

	m_currMapName = "";
	m_currMapScriptPath = "";
}