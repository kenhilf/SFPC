#ifndef _gamefont_h_
#define _gamefont_h_

#include <string>
#include "texture.h"

namespace FontStyle
{
	enum Type { FixedWidth, VariableWidth, NumTypes };
}

class FontHeader
{
public:
	FontHeader();
	bool Open(const std::string& filename);
	const std::string& GetFilename() const;
	char GetChar(const int i) const;
	int GetNumChars() const;
	int GetNumLines() const;

private:
	int m_numLines;
	std::string m_filename;
	std::string m_chars;
};

class GameFont
{
public:
	GameFont();
	void LoadFont(const FontHeader& fh);
	const Texture& GetFontElem(const char c) const;
	int Height() const;

private:
	bool m_bLoaded;	
	int m_height; // each char may have its own width, but height is constant

	// an array to hold all the textures that make up the font, indexed by their ascii value
    Texture m_fontElements[128];
};

class FontMgr
{
public:
	// returns singleton instance
	static FontMgr& Instance() { static FontMgr instance;  return instance; };
	void Init();
	GameFont* GetFont(const FontStyle::Type font);

private:
	FontMgr();
	FontMgr(const FontMgr& rhs);
	FontMgr& operator=(const FontMgr& rhs);

	GameFont m_fwFont;
	GameFont m_vwFont;
};

#endif // _gamefont_h_


