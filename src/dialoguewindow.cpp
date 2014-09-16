#include "dialoguewindow.h"
#include "inputmgr.h"
//#include "gamewindowmanager.h"

DialogueWindow::DialogueWindow()
{
//	m_x = 0;
//	m_y = 0;
	m_width = 0;
	m_height = 0;
	m_textX = 0;
	m_textY = 0;
	m_stringOffset = 0;

	m_winGfx = NULL;
	m_winFont = NULL;
	m_currLetter = 0;
//	SetLetterRate(32); // original rate before testing mouth movement
	SetLetterRate(24);
//	SetBlinkRate(1.75);

//	m_pFace = NULL;
//	m_pIconMenu = NULL;

//	m_bBlink = true;
	m_bUserConfirm = false;
	m_bUserAccept = false;
}

void DialogueWindow::Init(GameWindowSurfaces& GWS, GameFont& GF)
{
	m_winGfx = &GWS;
	m_winFont = &GF;

	m_letterTracker.Reset();
//	m_blinkTracker.Reset();
}
void SetLetterRate(double rate)
{
	m_letterTracker.SetRate(rate);
}
/*
void SetBlinkRate(double rate)
{
	m_blinkTracker.SetRate(rate); 
}
*/
/*
void SetPos(int x, int y)
{
	m_x = x;
	m_y = y;
	m_textX = m_x + 10;
	m_textY = m_y + 10;
}
*/
void DialogueWindow::DoUpdate(double ticks)
{
	m_letterTracker += ticks;
//	m_blinkTracker += ticks;

/*	int i = m_blinkTracker.GetUnitsAsInt();
	int j = 0;
	while (j < i)
	{
		m_bBlink = !m_bBlink;
		++j;
	}
	if (i > 0)
		m_blinkTracker.Reset();
 */ 
	int i = m_letterTracker.GetUnitsAsInt();
	int j = 0;
	while (j < i)
	{
		// HACK: odd bug during breakpoint testing for camera offset correction
		if (m_currLetter > (int)m_string.length()) 
			m_currLetter = (int)m_string.length();

		if (m_string[m_currLetter] != PAUSE_CHAR)
		{
			m_currLetter++;
		}
		++j;
	}

	if (m_currLetter > (int)m_string.length()) 
		m_currLetter = (int)m_string.length();

	if (m_bUserAccept && (m_string[m_currLetter] == PAUSE_CHAR))
	{
		++m_currLetter;
		m_stringOffset = m_currLetter;
		m_bUserAccept = false;
		m_bUserConfirm = false;
	}
}

void DoDraw(DrawMgr& drawMgr)
{
	DrawFrame(drawMgr);
	DrawString(drawMgr);
}

void DialogueWindow::ProcessInput(InputMgr& input)
{
	GameWindowManager& gwm = GetGameWindowManager();

	// Spacebar to advance text, provided the blinking triangle is up
	if(input.WasPressed(VK_SPACE) && WaitingForUserConfirmation())
		UserAccept();

	if(input.WasPressed(VK_SPACE) && AtEndOfString())
	{
		RestartText();
		gwm.CloseTextWindow();
	}
}

void DialogueWindow::SetString(const char text[])
{
	// Set the string and precalculate which spaces to start new lines on

	// set all positions back to default and store the string into the member string
	m_currLetter = 0;
	m_textX = m_x + 10; 
	m_textY = m_y + 10;
	m_string = text;

	UINT i = 0;
	int strWidth = 0;
	const int NO_SPACE = -1;
	int lastSpace = NO_SPACE;

	// find all spaces, when a word goes over on size, go back to the last good space, 
	// replace it with a \n, and start the new line on the next word
	while(i < m_string.length())
	{
		strWidth += m_winFont->GetFontElem(m_string[i]).Width();
		if (m_string[i] == ' ')
		{
			lastSpace = i;
		}
		// experimental user added \n detection
		if (m_string[i] == EOL_CHAR || m_string[i] == PAUSE_CHAR)
		{
			lastSpace = NO_SPACE;
			strWidth = 0;
		}
		// end experimental user added \n detection

		if (strWidth > (m_width - 10))
		{
			assert(lastSpace != NO_SPACE);
			i = lastSpace;
			m_string[i] = EOL_CHAR;
			lastSpace = NO_SPACE;
			strWidth = 0;
		}
		++i;
	}
}

void DialogueWindow::DrawString(DrawMgr& drawMgr)
{
	int i = m_stringOffset;
	int xpos = m_textX;
	int ypos = m_textY;

	while(i < m_currLetter)
	{
		// if at an endline, adjust positions accordingly
		if (m_string[i] == EOL_CHAR)
		{
			xpos = m_textX;
			ypos += m_winFont->Height();
			++i;  // skip this space
		}
		assert(m_string[i] != PAUSE_CHAR);
		
		DrawChar(m_string[i], xpos, ypos, drawMgr);

		xpos += m_winFont->GetFontElem(m_string[i]).Width();
		++i;
	}
	
	if (m_pFace != NULL)
	{
        if (m_string[i] == ' ')
			m_pFace->CloseMouth();
		else
			m_pFace->OpenMouth();
	}

	if (m_string[i] == PAUSE_CHAR)
	{
		m_bUserConfirm = true;
		if (m_pFace != NULL)
		{
			m_pFace->CloseMouth();
		}
	}

	if ((i == m_string.size()) && (m_pFace != NULL))
		m_pFace->CloseMouth();

	if ((m_string[i] == PAUSE_CHAR) && m_bBlink)
		DrawChar(m_string[i], xpos, ypos, drawMgr);
}

void DialogueWindow::DrawChar(int chr, int x, int y, DrawMgr& drawMgr)
{
//	drawMgr.Draw(m_winFont->GetFontElem(chr).GetSurface(), Position(x+1, y+1), DrawSpace::Screen);
	drawMgr.Draw(m_winFont->GetFontElem(chr).GetSurface(), Position(x, y), DrawSpace::Screen);
}

void DialogueWindow::SetFace(Face* pFace)
{
	if (pFace != NULL)
	{
		AddChild(*pFace);
		m_pFace = pFace;
		m_pFace->SetWorldPos(Position(0, 240));
	}
	else // pFace == NULL
	{
		RemoveChild(*m_pFace);
		m_pFace = pFace;
	}
}

void DialogueWindow::SetIconMenu(IconMenu* pIconMenu)
{
	if (pIconMenu != NULL)
	{
		AddChild(*pIconMenu);
		m_pIconMenu = pIconMenu;
//		m_pIconMenu->SetLocalPos(Position(-150, -24));
		m_pIconMenu->SetLocalPos(Position(256, 360));
	}
	else // pIconMenu == NULL
	{
		RemoveChild(*m_pIconMenu);
		m_pIconMenu = pIconMenu;
	}
}

bool DialogueWindow::AtEndOfString()
{
	if (m_currLetter == m_string.size())
		return true;
	else
		return false;
}

void DialogueWindow::RestartText()
{
	m_currLetter = 0;
	m_stringOffset = 0;
	m_letterTracker.Reset();
	m_blinkTracker.Reset();
}

/* old ProcessInput
void DialogueWindow::ProcessInput(InputMgr& input)
{
	GameWindowManager& gwm = GetGameWindowManager();

	if (m_pIconMenu == NULL)
	{
		// Spacebar to advance text, provided the blinking triangle is up
		if(input.WasPressed(VK_SPACE) && WaitingForUserConfirmation())
			UserAccept();

		if(input.WasPressed(VK_SPACE) && AtEndOfString())
		{
			RestartText();
			gwm.CloseTextWindow();
		}

		if (input.WasPressed(VK_TAB))
		{
			static DialogueWindow extraWindow;
			extraWindow.Init(gwm.TEMP_GetGWS(), gwm.TEMP_GetGFNormal());
			extraWindow.SetPos(180, 160);
			extraWindow.SetSize(440, 100);
			extraWindow.SetString("Dayla: Two windows are better than one!^I'm liking this, it means the stack is working!^And I agree, the pink halo has got to go.");

			gwm.PushModalWindow(extraWindow);
		}
	}
	else
	{
		m_pIconMenu->ProcessInput(input, gwm);
	}
}

*/