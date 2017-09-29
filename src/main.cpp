/*! 
    \brief Main function

    Game entry point.
*/

#include <windows.h>
#include "splash.h"
#include "menu.h"
#include "help.h"
#include "board.h"

static HICON hIcon = NULL;

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT) {

	Diamondek::Splash* pSplash;
	Diamondek::Menu* pMenu;
	Diamondek::Help* pHelp;
	Diamondek::Board* pBoard;

	sf::Color textColor;

	// Create the window of the application
	sf::RenderWindow gameWindow(sf::VideoMode(RESOLUTION_X, RESOLUTION_Y, 32), "Diamondek", sf::Style::Titlebar);
	//gameWindow.setVerticalSyncEnabled(true);
	//hIcon = LoadIcon(hInst, "IDI_ICON1");
	//SendMessage(0, WM_SETICON, ICON_BIG, (LPARAM) hIcon);

	// Show splash screen
	pSplash = new Diamondek::Splash(gameWindow);
	try
	{
		pSplash->loadResources();
	}
	catch(char* s)
	{
		s; // Do something...
	};
	pSplash->run();
	delete pSplash;
	
	// Prepare menu and run application loop
	pMenu = new Diamondek::Menu(gameWindow);
	pMenu->loadResources();
	while(1)
	{
		switch (pMenu->run())
		{
			// Run game from first level
			case Diamondek::maNewGame:
				pBoard = new Diamondek::Board("data/board_bkg.png");
				pBoard->run(gameWindow);
				delete pBoard;
				break;
			// Enter code and run game from appropriate level
			case Diamondek::maEnterCode:
				break;
			// Show game credits
			case Diamondek::maHelp:
				pHelp = new Diamondek::Help(gameWindow);
				pHelp->loadResources();
				pHelp->run();
				delete pHelp;
				break;
			// Exit game application
			case Diamondek::maExit:
				gameWindow.close();
				DestroyIcon(hIcon);
				return EXIT_SUCCESS;
				break;
		};
	};
    return EXIT_SUCCESS;
}
