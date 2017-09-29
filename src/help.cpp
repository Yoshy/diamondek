/*! 
	\class Diamondek::Help
    \brief Help screen class

    Help screen is displayed, when selected from menu.
*/

#include "help.h"

namespace Diamondek {

Help::Help(sf::RenderWindow &gameWindow)
{
	_gameWindow = &gameWindow;
};

Help::~Help()
{
};

void Help::loadResources()
{
	if (_bkgImage.loadFromFile("data/help.png") == false) throw "Error loading image help.png";
	_bkg.setTexture(_bkgImage);
};

void Help::waitForEvent()
{
    sf::Event Event;
    while (_gameWindow->waitEvent(Event))
    {
		switch(Event.type)
		{
			case sf::Event::Closed:
			case sf::Event::KeyPressed:
				return;
				break;
		};
	};
};

void Help::run()
{
	draw();
	waitForEvent();
};

void Help::draw()
{
	_gameWindow->clear();
	_gameWindow->draw(_bkg);
	_gameWindow->display();
};

}; // namespace Diamondek