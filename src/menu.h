/*! 
	\class Diamondek::Menu
    \brief Menu class

    Main menu.
*/

#ifndef _MENU_H_
#define _MENU_H_

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#define MENU_FONT_SIZE 60
#define MENU_POSITION_X 100
#define MENU_POSITION_Y 200
#define MENU1 "Start game"
#define MENU2 "Enter level code"
#define MENU3 "Help"
#define MENU4 "Exit"

namespace Diamondek {

typedef sf::Text* pMenuItem;
typedef enum { maNone, maEnter, maNewGame, maEnterCode, maHelp, maExit } menuActions;

class Menu
{
public:
    Menu(sf::RenderWindow &gameWindow);
	~Menu();
	/// Process system events
	menuActions processEvents();
	void loadResources();
	void addItem(std::string text);
	menuActions run();
	void draw();
private:
	bool _isRunning;
	uint32_t _activeItem;
	std::vector<pMenuItem> _items;
	sf::Font _menuFont;
	sf::Texture _bkgImage;
	sf::Sprite _bkg;
	sf::Music _music;
	sf::SoundBuffer _menuMoveSoundBuffer;
	sf::SoundBuffer _menuSelectSoundBuffer;
	sf::Sound _menuMoveSound;
	sf::Sound _menuSelectSound;
	sf::RenderWindow* _gameWindow;
};

}; // namespace Diamondek

#endif // _MENU_H_
