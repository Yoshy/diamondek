/*! 
	\class Diamondek::Splash
    \brief Splash screen class

    Splash screen is displayed once during game startup.
*/

#include "splash.h"

namespace Diamondek {

Splash::Splash(sf::RenderWindow &gameWindow)
{
	_gameWindow = &gameWindow;
};

Splash::~Splash()
{
};

void Splash::loadResources()
{
	if (_splash1.loadFromFile(SPLASH1) == false) throw "Error loading image " SPLASH1;
	if (_splash2.loadFromFile(SPLASH2) == false) throw "Error loading image " SPLASH2;
	if (_splash3.loadFromFile(SPLASH3) == false) throw "Error loading image " SPLASH3;

	_buffer.create(RESOLUTION_X, RESOLUTION_Y);
	_sprite.setTexture(_buffer);
};

void Splash::processEvents()
{
    sf::Event Event;
    while (_gameWindow->pollEvent(Event))
    {
		switch(Event.type)
		{
			// Window closed, shutdown game
			case sf::Event::Closed:
				_isRunning = false;
				break;
			case sf::Event::KeyPressed:
				_isRunning = false;
				break;
			case sf::Event::MouseButtonPressed:
				_isRunning = false;
				break;
		};
	};
};

void Splash::run()
{
	_isRunning = true;
	showImageByFadeTransition(_splash1, 1, 3, 1);
	if (_isRunning) showImageByFadeTransition(_splash2, 1, 3, 1);
	if (_isRunning) showImageByFadeTransition(_splash3, 1, 1, 0);
};

void Splash::showImageByFadeTransition(sf::Image& img, float fadeOutTime, float showTime, float fadeInTime)
{
	sf::Clock clk;
	float time;

	clk.restart();
	while ((clk.getElapsedTime().asSeconds() < (fadeOutTime + showTime + fadeInTime)) && _isRunning)
	{
		processEvents();
		_gameWindow->clear();
		time = clk.getElapsedTime().asSeconds();
		// Fade out
		if ((fadeOutTime!=0) && (time < fadeOutTime))
		{
			fadeImage(_buffer, img, 1 - (fadeOutTime - time) / fadeOutTime);
		};
		// Show still image
		if ((showTime!=0) && (time >= fadeOutTime) && (time < (fadeOutTime + showTime)))
		{
			fadeImage(_buffer, img, showTime);
		};
		// Fade in black
		if ((fadeInTime!=0) && (time >= (fadeOutTime + showTime)) && (time < (fadeOutTime + showTime + fadeInTime)))
		{
			fadeImage(_buffer, img, (fadeInTime + fadeOutTime + showTime - time) / fadeInTime);
		};
		_gameWindow->draw(_sprite);
		_gameWindow->display();
	}; //while
};

void Splash::fadeImage(sf::Texture& dst, const sf::Image& src, float amount)
{
	uint32_t sx = dst.getSize().x;
	uint32_t sy = dst.getSize().y;
	sf::Color c;
	sf::Image tmpImg;

	tmpImg.create(sx, sy);
	for (uint32_t y = 0; y < sy; y++)
		for (uint32_t x = 0; x < sx; x++)
		{
			c = src.getPixel(x, y);
			c.a = (sf::Uint8)(255 * amount);
			tmpImg.setPixel(x, y, c);
		};
	dst.update(tmpImg);
};

}; // namespace Diamondek