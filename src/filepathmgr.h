#ifndef _filepathmgr_h_
#define _filepathmgr_h_

#include <string>

class FilePathMgr
{
public:
	// returns singleton instance
	static FilePathMgr& Instance() { static FilePathMgr instance;  return instance; };
	~FilePathMgr();

	const std::string& GetCurrMapName() const;
	void SetCurrMapName(const std::string& mapname);

	const std::string& GetBasePath() const;
	const std::string& GetTilePath() const;
	const std::string& GetImagePath() const;
	const std::string& GetScriptPath() const;
	const std::string& GetMapPath() const;
	const std::string& GetMapScriptPath() const;
	const std::string& GetFontPath() const;
	const std::string& GetSoundPath() const;

private:
	FilePathMgr();
	FilePathMgr(const FilePathMgr& rhs);
	FilePathMgr& operator=(const FilePathMgr& rhs);

	std::string m_basePath;
	std::string m_tilePath;
	std::string m_imagePath;
	std::string m_scriptPath;
	std::string m_mapPath;
	std::string m_fontPath;
	std::string m_soundPath;

	std::string m_currMapName;
	std::string m_currMapScriptPath;
};

#endif //_filepathmgr_h_