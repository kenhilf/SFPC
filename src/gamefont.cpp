#include "gamefont.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include "filepathmgr.h"

//////////////////////
// FontHeader       //
//////////////////////

FontHeader::FontHeader() :
	m_numLines(0)
{

}

bool FontHeader::Open(const std::string& filename)
{
	std::ifstream fin;
	fin.open(filename.c_str());
	if (!fin)
	{
		assert(false && "Failed to open specified font header.");
	}

	char line[1024];
	fin.getline(line, sizeof(line), '\n');
	std::stringstream stream1;
	stream1 << line;
	stream1 >> m_filename;

	fin.getline(line, sizeof(line), '\n');
	std::stringstream stream2;
	stream2 << line;
	stream2 >> m_numLines;

	while (fin.getline(line, sizeof(line), '\n'))
	{
		m_chars += line;
		m_chars += '\n';  // for new line for multi line fonts
	}
	return true;
}

const std::string& FontHeader::GetFilename() const
{
	return m_filename; 
}

char FontHeader::GetChar(const int i) const
{
	return m_chars[i]; 
}

int FontHeader::GetNumChars() const
{
	return m_chars.length(); 
}

int FontHeader::GetNumLines() const
{
	return m_numLines; 
}

//////////////////////
// GameFont 		//
//////////////////////

GameFont::GameFont() :
	m_bLoaded(false),	
	m_height(0)
{

}

void GameFont::LoadFont(const FontHeader& fh)
{
	Image fontMaster;
	const std::string fontBmpPath = FilePathMgr::Instance().GetImagePath() + fh.GetFilename();
	fontMaster.LoadBMP(fontBmpPath);
	
	unsigned int* master = (unsigned int*)fontMaster.m_pImg;
	unsigned int border = (unsigned int)master[0];
	unsigned int pitch = (unsigned int)fontMaster.m_width;

	// set the pointer to the first non-border pixel at (1, 1)
	unsigned int* temp = master + pitch + 1;  

	int chrHeight = 0;
	while (temp[0] != border)
	{
		++chrHeight;
		temp += pitch;
		if (chrHeight > fontMaster.m_height) 
			assert(false && "Failure to load font, error in height determining phase.");
	}
	m_height = chrHeight;

	int chrWidth = 0;
	int x = 1;
	int y = 1;
	int currline = 0;
	temp = master + pitch + 1; // reset to (1, 1))
	for (int i = 0; i < fh.GetNumChars(); ++i)
	{
		if (fh.GetChar(i) == '\n')
		{
			y += chrHeight + 1;
			x = 1;
			++currline;
			temp = master + (y * pitch) + x;
			++i;
			if (currline >= fh.GetNumLines()) break;
		}

		while(temp[0] != border)
		{
			++chrWidth;
			++temp;
			if (chrWidth > fontMaster.m_width)
				assert(false && "Error in font loading, character exceeds width of font surface.");
		}

		m_fontElements[fh.GetChar(i)].LoadSubImage(fontMaster, x, y, chrWidth, chrHeight);
		++temp;
		x += chrWidth + 1;
		chrWidth = 0;
	}

	m_bLoaded = true;
}

const Texture& GameFont::GetFontElem(const char c) const
{
	return m_fontElements[c];
}

int GameFont::Height() const
{
	return m_height;
}

//////////////////////
// FontMgr			//
//////////////////////

FontMgr::FontMgr()
{

}

void FontMgr::Init()
{
	FontHeader vwfh;
	std::string vwpath = FilePathMgr::Instance().GetFontPath() + "sffontheader.fh";
	vwfh.Open(vwpath.c_str());
	m_vwFont.LoadFont(vwfh);

	FontHeader fwfh;
	std::string fwpath = FilePathMgr::Instance().GetFontPath() + "sfblockfontheader.fh";
	fwfh.Open(fwpath.c_str());
	m_fwFont.LoadFont(fwfh);
}

GameFont* FontMgr::GetFont(const FontStyle::Type font)
{
	switch(font)
	{
		case FontStyle::FixedWidth:		return &m_fwFont;
		case FontStyle::VariableWidth:	return &m_vwFont;
	}

	assert(false && "Error obtaining GameFont, requested font does not exist!");
	return NULL;
}