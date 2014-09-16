#ifndef _dialoguewindow_h_
#define _dialoguewindow_h_

#include "gamewindow.h"
#include "gametimer.h"
#include "gamestring.h"
//#include "face.h"
//#include "iconmenu.h"

class DialogueWindow : public GameWindow
{
public:
	DialogueWindow();

//	virtual void Init(GameWindowSurfaces& GWS, GameFont& GF);

	// rate in letters per second
	void SetLetterRate(double rate);
	// rate in blinks per second
//	void SetBlinkRate(double rate);
	// need to use GameObject fns.  strings as children may make this obsolete
//	virtual void SetPos(int x, int y);
	void SetString(const char text[]);
	void DrawString(DrawMgr& drawMgr);
	void DrawChar(int chr, int x, int y, DrawMgr& drawMgr);

	virtual void DoUpdate(double ticks);
	virtual void DoDraw(DrawMgr& drawMgr);
	virtual void ProcessInput(InputMgr& input);

//	void SetFace(Face* face);

//	void SetIconMenu(IconMenu* pIconMenu);

	bool AtEndOfString();
	void RestartText();
	bool WaitingForUserConfirmation() { return m_bUserConfirm; }
	void UserAccept() { m_bUserAccept = true; }

private:
	const static char EOL_CHAR = '\n';
	const static char PAUSE_CHAR = '^';

	int m_textX, m_textY;
	int m_currLetter;
	int m_stringOffset;

	bool m_bBlink;
	bool m_bUserConfirm;
	bool m_bUserAccept;

//	Face* m_pFace;

//	IconMenu* m_pIconMenu;

	std::string m_string;

	GameTimer::Tracker m_letterTracker;
//	GameTimer::Tracker m_blinkTracker;
};

#endif //_dialoguewindow_h_


