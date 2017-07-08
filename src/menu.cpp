// (C) 2011 PowerBIT Studio
/*! 
	\class Diamondek::Menu
    \brief Menu class

    Main menu.
*/

#include "menu.h"

namespace Diamondek {

Menu::Menu(sf::RenderWindow &gameWindow)
{
	_gameWindow = &gameWindow;
	_activeItem = 0;
};

Menu::~Menu()
{
	pMenuItem tmp;
	for (std::vector<pMenuItem>::iterator i = _items.begin(); i != _items.end(); i++)
	{
		tmp = *i;
		delete tmp;
	};
	_items.clear();
};

menuActions Menu::processEvents()
{
    sf::Event Event;
    while (_gameWindow->waitEvent(Event))
    {
		switch(Event.type)
		{
			// Window closed, shutdown game
			case sf::Event::Closed:
				_isRunning = false;
				break;
			case sf::Event::KeyPressed:
				switch (Event.key.code)
				{
					case sf::Keyboard::Return:
						_menuSelectSound.play();
						//while (_menuSelectSound.getStatus() != sf::Sound::Stopped);
						return maEnter;
					case sf::Keyboard::Down:
						_activeItem++;
						if (_activeItem == _items.size()) _activeItem = 0;
						_menuMoveSound.play();
						return maNone;
						break;
					case sf::Keyboard::Up:
						if (_activeItem == 0) _activeItem = _items.size() - 1; else _activeItem--;
						_menuMoveSound.play();
						//while (sound.getStatus() != sf::Sound::Stopped);
						return maNone;
						break;
				};
				break;
		};
	};
	return maNone;
};

void Menu::loadResources()
{
	if (!_menuFont.loadFromFile("data/davis.ttf")) throw "Error loading font 'davis.ttf'";
	if (_bkgImage.loadFromFile("data/menu_background.png") == false) throw "Error loading image 'menu_background.png'";
	if (!_music.openFromFile("data/sounds/menu_music.ogg")) throw "Error loading music 'menu_music.ogg'";
	if (!_menuMoveSoundBuffer.loadFromFile("data/sounds/menu_move.ogg")) throw "Error loading sound 'menu_move.ogg'";
	if (!_menuSelectSoundBuffer.loadFromFile("data/sounds/menu_select.ogg")) throw "Error loading sound 'menu_move.ogg'";
	_menuMoveSound.setBuffer(_menuMoveSoundBuffer);
	_menuSelectSound.setBuffer(_menuSelectSoundBuffer);

	_bkg.setTexture(_bkgImage);
	addItem(MENU1);
	addItem(MENU2);
	addItem(MENU3);
	addItem(MENU4);
};

void Menu::addItem(std::string text)
{
	pMenuItem i;
	uint32_t yOffset = _items.size();

	i = new sf::Text(text, _menuFont, MENU_FONT_SIZE);
	i->setPosition(MENU_POSITION_X, MENU_POSITION_Y + (float)(MENU_FONT_SIZE * 1.1 * yOffset));
	_items.push_back(i);
};

menuActions Menu::run()
{
	_music.play(); _music.setVolume(20);
	draw();
	_isRunning = true;
	while (_isRunning)
	{
		if (processEvents() == maEnter)
		{
			if (_activeItem == 0)
			{
				_music.stop();
				return maNewGame;
			};
			if (_activeItem == 1)
			{
				return maEnterCode;
			};
			if (_activeItem == 2)
			{
				return maHelp;
			};
			if (_activeItem == 3)
			{
				_music.stop();
				_menuMoveSound.stop();
				_menuSelectSound.stop();
				return maExit;
			};
		};
		draw();
	};
	return maNone;
};

void Menu::draw()
{
	uint32_t cnt = 0;
	_gameWindow->clear();
	_gameWindow->draw(_bkg);
	for (std::vector<pMenuItem>::iterator i = _items.begin(); i != _items.end(); i++)
	{
		if (cnt == _activeItem) (*i)->setFillColor(sf::Color::Black); else (*i)->setFillColor(sf::Color::White);
		_gameWindow->draw(*(*i));
		cnt++;
	};
	_gameWindow->display();
};

}; // namespace Diamondek