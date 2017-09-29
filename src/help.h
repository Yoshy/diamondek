/*! 
	\class Diamondek::Help
    \brief Help screen class

    Help screen is displayed, when selected from menu.

*/

#ifndef _HELP_H_
#define _HELP_H_

#include <SFML/Graphics.hpp>

namespace Diamondek {

class Help
{
public:
    Help(sf::RenderWindow &gameWindow);
	~Help();
	void loadResources();
	void run();
	void waitForEvent();
	void draw();
private:
	sf::Texture _bkgImage;
	sf::Sprite _bkg;
	sf::RenderWindow* _gameWindow;
};

}; // namespace Diamondek

#endif // _HELP_H_
