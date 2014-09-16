#include "gamestring.h"
#include "gamefont.h"

GameString::GameString(const std::string& text) :
	GameObject(LayerSpace::Window),
	m_string(text),
	m_pgf(NULL),
	m_scale(0.5f),
	m_drawSpace(DrawSpace::Screen),
	m_firstChar(0),
	m_lastChar(0),
	m_lineWidth(GAME_SCREEN_WIDTH),
	m_tintRed(1.0f),
	m_tintGreen(1.0f),
	m_tintBlue(1.0f),
	m_offX(0),
	m_offY(0)
{
	m_pgf = FontMgr::Instance().GetFont(FontStyle::VariableWidth);
	ClearBounds();
	m_slider.SetSlideRate(4.0f);
	AddChild(m_slider);
}

GameString::~GameString()
{

}

void GameString::SetFont(GameFont* const pgf)
{
	m_pgf = pgf;
}

void GameString::SetString(const std::string& text)
{
	m_string = text;
	m_firstChar = 0;
	m_lastChar = static_cast<int>(m_string.length());
	ProcessString();
}

void GameString::SetOffset(const int x, const int y)
{
	m_offX = x;
	m_offY = y;
}

void GameString::SetDrawSpace(const DrawSpace::Type ds)
{
	m_drawSpace = ds;
}

void GameString::SetScale(const float scale)
{
	assert(scale >= 0.1f);
	m_scale = scale;
}

void GameString::SetFirstChar(const int fc)
{
	assert((fc >= 0) && (fc <= m_lastChar));
	m_firstChar = fc;
}

void GameString::SetLastChar(const unsigned int lc)
{
	assert(lc <= m_string.length());
	m_lastChar = lc;
}

void GameString::SetTint(const float r, const float g, const float b)
{
	m_tintRed = r;
	m_tintGreen = g;
	m_tintBlue = b;
}

void GameString::SetBounds(const int x, const int y, const int width, const int height)
{
	//@TODO: the +8 and -14 keep text in a window from overlapping on the window frame,
	// may want to make these defines instead
	m_bHasBounds = true;
	m_boundX = x;
	m_boundY = y + 8;
	m_boundW = width;
	m_boundH = height - 14;
}

void GameString::UpdateBounds(const int x, const int y, const int width, const int height)
{
	m_boundX = x;
	m_boundY = y + 8;
	m_boundW = width;
	m_boundH = height - 14;
}

void GameString::ClearBounds()
{
	m_bHasBounds = false;
	m_boundX = m_boundY = m_boundW = m_boundH = 0;
}

const char GameString::GetChar(const unsigned int index) const
{
	assert((index >= 0) && (index <= m_string.length()));
	return m_string[index];
}

const int GameString::GetFirstChar() const
{
	return m_firstChar;
}

const int GameString::GetLastChar() const
{
	return m_lastChar;
}

const int GameString::GetStringLength() const
{
	return static_cast<int>(m_string.length());
}

void GameString::SetLineWidth(const int lw)
{
	m_lineWidth = lw;
}

void GameString::ScrollLine(const Direction::Type dir, const int numlines)
{
	int scrollDist = static_cast<int>(m_pgf->Height() * m_scale * numlines);
	m_slider.SlidePixels(dir, scrollDist);
}

const bool GameString::IsScrolling() const
{
	return !(m_slider.AtDest());
}

void GameString::ResetScrolling()
{
	m_slider.SnapOntoScreen();
}

void GameString::DoUpdate(const float ticks)
{
	Vector2f pos = m_slider.GetLocalPos();
	pos.x += m_offX;
	pos.y += m_offY;
	SetLocalPos(pos);
}

void GameString::DoDraw(DrawMgr& drawMgr)
{
	float xpos = GetWorldPos().x;
	float ypos = GetWorldPos().y;

	if (m_bHasBounds)
		drawMgr.BeginScissor(m_boundX, m_boundY, m_boundW, m_boundH);

	SetTint(1.0f, 1.0f, 1.0f);
	for (int i = m_firstChar; i < m_lastChar; i++)
	{
		if ((m_string[i] == EOL_CHAR) || (m_string[i] == SCRIPT_EOL_CHAR))
		{
			xpos = GetWorldPos().x;
			ypos += static_cast<int>(m_pgf->Height() * m_scale);
			++i;  // skip drawing this character
			if (i == m_lastChar)
				break;
		}
		if (m_string[i] == SCROLL_CHAR)
		{
			m_string.replace(i, 1, " ");
			ScrollLine();
			++i;  // skip drawing this character
			if (i == m_lastChar)
				break;
		}
		if (m_string[i] == TINT_CHAR)
		{
			++i;  // skip drawing this character
			const char* temp = &m_string[i];
			int color = atoi(temp);
			switch(color)
			{
				case 0: // White/No tint
					SetTint(1.0f, 1.0f, 1.0f);
				break;
				case 1: // Orange (common in SF2)
					SetTint(1.0f, 0.5f, 0.0f);
				break;
				case 2: // Red
					SetTint(1.0f, 0.0f, 0.0f);
				break;
				case 3: // Blue
					SetTint(0.0f, 0.0f, 1.0f);
				break;
				case 4: // Yellow
					SetTint(1.0f, 1.0f, 0.0f);
				break;
				case 5: // Green
					SetTint(0.0f, 1.0f, 0.0f);
				break;
				case 6: // Purple
					SetTint(0.75f, 0.1f, 0.75f);
				break;
				case 7: // Grey
					SetTint(0.5f, 0.25f, 0.0f);
				break;
				case 8: // Brown
					SetTint(0.5f, 0.5f, 0.5f);
				break;
				case 9: // Black
					SetTint(0.0f, 0.0f, 0.0f);
				break;
				default:
					assert(false && "Invalid color mode in GameString!");
				break;
			}
			++i;
			if (i == m_lastChar)
				break;
		}

		DrawChar(m_string[i], xpos, ypos, drawMgr);
		xpos += static_cast<int>(m_pgf->GetFontElem(m_string[i]).m_dataWidth * m_scale);
	}

	if (m_bHasBounds)
		drawMgr.EndScissor();
}

void GameString::DrawChar(const int chr, const float x, const float y, DrawMgr& drawMgr)
{
	drawMgr.OneTimeTint(0.2f, 0.2f, 0.2f);
	drawMgr.DrawQuad(m_pgf->GetFontElem(chr), Vector2f(x+1, y+1), m_drawSpace, FlipMode::None, ScaleMode::Percentage, m_scale, m_scale);
	drawMgr.OneTimeTint(m_tintRed, m_tintGreen, m_tintBlue);
	drawMgr.DrawQuad(m_pgf->GetFontElem(chr), Vector2f(x, y), m_drawSpace, FlipMode::None, ScaleMode::Percentage, m_scale, m_scale);
}

// calculates which spaces to start new lines on
void GameString::ProcessString()
{
	int i = 0;
	int stringPixelWidth = 0;
	const int NO_SPACE = -1;
	int lastSpace = NO_SPACE;

	// find all spaces, when a word goes over on size, go back to the last good space, 
	// replace it with a \n, and start the new line on the next word
	while(i < static_cast<int>(m_string.length()))
	{
		stringPixelWidth += static_cast<int>(m_pgf->GetFontElem(m_string[i]).m_dataWidth * m_scale);
		if (m_string[i] == ' ')
		{
			lastSpace = i;
		}
		if (m_string[i] == TINT_CHAR)
		{
			i += 1;
		}
		if ((m_string[i] == EOL_CHAR) || 
			(m_string[i] == PAUSE_CHAR) || 
			(m_string[i] == SCRIPT_EOL_CHAR))
		{
			lastSpace = NO_SPACE;
			stringPixelWidth = 0;
		}
		if ((stringPixelWidth > m_lineWidth) && (lastSpace != NO_SPACE))
		{
			i = lastSpace;
			m_string[i] = EOL_CHAR;
			lastSpace = NO_SPACE;
			stringPixelWidth = 0;
		}
		++i;
	}
}