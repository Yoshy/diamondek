// (C) 2011 PowerBIT Studio
/*! 
	\class Diamondek::Splash
    \brief Splash screen class

    Splash screen is displayed once during game startup.
	1. FadeOut from black to "PowerBIT Studio presents"
	2. FadeIn to black
	3. FadeOut from black to "Diamondek" cool stylized logo
	4. FadeIn to black
	5. FadeOut from black to Main menu background
	6. Exit
*/

#ifndef _SPLASH_H_
#define _SPLASH_H_

#define SPLASH1 "data/splash1.png"
#define SPLASH2 "data/splash2.png"
#define SPLASH3 "data/menu_background.png"

#include "globals.h"
#include <SFML/Graphics.hpp>
#include <boost/lexical_cast.hpp>

namespace Diamondek {

class Splash
{
public:
    Splash(sf::RenderWindow &gameWindow);
	~Splash();
	void loadResources();
	void run();
	/// Process system events
	void processEvents();
	/// Make image in dst with src, faded from black in amount (0 - pure black, 1 - dst=src)
	void fadeImage(sf::Texture& dst, const sf::Image& src, float amount);
	/// Fade-out image from the black, wait and fade-in back to the black. Zero time in any parameter mean no transition
	void showImageByFadeTransition(sf::Image& img, float fadeOutTime, float showTime, float fadeInTime);
private:
	bool _isRunning;
	sf::Texture _buffer;
	sf::Image _splash1, _splash2, _splash3;
	sf::Sprite _sprite;
	sf::RenderWindow* _gameWindow;
	sf::Clock _clock;
};

}; // namespace Diamondek

#endif // _SPLASH_H_
