/*! 
	\class Diamondek::Board
    \brief Board class

    Representing 2D space.
*/

#pragma warning( disable : 4290 )
#pragma warning( disable : 4099 )

#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <cmath>
#include <random>
#include <stdlib.h>
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/error/en.h"
#include <string>
#include "board.h"

namespace Diamondek {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Board::Board(const std::string& backgroundSpriteName)
{
	_nextID = 1;
	_diamondsGained = 0;
	_numDiamonds = 0;
	_numLives = LIVES_MAX;
	_currentLevel = 1;
	_background.loadFromFile(backgroundSpriteName);

	// Load font and init some strings
	_font.loadFromFile("data/NovaSquare.ttf");
	// Load sounds
	if (!_music.openFromFile("data/sounds/game_music3.ogg")) throw "Error loading music 'game_music.ogg'";
	if (!_ballHitSoundBuffer.loadFromFile("data/sounds/game_ballhit.wav")) throw "Error loading music 'game_ballhit.wav'";
	_ballHitSound.setBuffer(_ballHitSoundBuffer);
	if (!_explodeSoundBuffer.loadFromFile("data/sounds/game_explode.wav")) throw "Error loading music 'game_explode.wav'";
	_explodeSound.setBuffer(_explodeSoundBuffer);
	if (!_harvestSoundBuffer.loadFromFile("data/sounds/game_harvest.wav")) throw "Error loading music 'game_harvest.wav'";
	_harvestSound.setBuffer(_harvestSoundBuffer);
	// Prepare some Texts
	_sNumGems.setFont(_font);
	_sNumGems.setPosition(GEMS_TEXT_X, GEMS_TEXT_Y);
	_sNumGems.setScale(0.5, 0.5);
	_sNumLives.setFont(_font);
	_sNumLives.setPosition(LIVES_TEXT_X, LIVES_TEXT_Y);
	_sNumLives.setScale(0.5, 0.5);
	_sLevelInfo.setFont(_font);
	_sLevelInfo.setPosition(LEVEL_INFO_TEXT_X, LEVEL_INFO_TEXT_Y);
	_sLevelInfo.setScale(0.5, 0.5);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Board::~Board()
{
	_clearSpritexes();
};

void Board::_clearSpritexes()
{
	Spritex* s;
	SpritexMapIterator i = _spritexes.begin();

	while(!_spritexes.empty())
	{
		s = (*i).second;
		_spritexes.erase(i++);
		delete(s);
	};
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t Board::addSpritex(Spritex* s)
{
	/// \todo Implement check for duplicates
	_spritexes[_nextID] = s;
	++_nextID;
	return _nextID-1;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Board::removeSpritex(Spritex* s)
{
	Spritex* cur;
	SpritexMapIterator i = _spritexes.begin();
	while(!_spritexes.empty())
	{
		cur = (*i).second;
		if (cur == s)
		{
			cur->setDead();
			return;
		};
		++i;
	};
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Spritex* Board::getSpritex(uint32_t id)
{
	return _spritexes[id];
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Board::removeCollidingBackground(Spritex* s)
{
	for (SpritexMapIterator iter = _spritexes.begin(); iter != _spritexes.end(); ++iter)
    {
		if (((*iter).second) == s) continue;
		s->collides(*((*iter).second), true, true, NULL);
	};
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Board::drawBoard(sf::RenderTarget& target)
{
	sf::Sprite s;
	s.setTexture(_background);
	target.draw(s);
	for (SpritexMapIterator i = _spritexes.begin(); i != _spritexes.end(); ++i) {
		((*i).second)->draw(target, sf::RenderStates::Default);
	};
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Board::setPaddleSpeed(sf::Vector2f speed)
{
	getSpritex(_paddleID)->setSpeed(speed);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Board::setBallSpeed(sf::Vector2f speed)
{
	getSpritex(_ballID)->setSpeed(speed);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Board::processSpritexes()
{
	sf::Vector2f vel, rvect, curPos;
	CollisionData collisionData;

	for (SpritexMapIterator i = _spritexes.begin(); i != _spritexes.end();)
	{
		if (((*i).second->isDynamic()))
		{
			// Try to move spritex
			curPos = ((*i).second)->getPosition();
			(*i).second->physicsTick();
			// Check for collision
			if (_findCollision((*i).second, &collisionData))
			{
				// Move spritex to position right before collision
				rvect = (*i).second->getSpeed();
				if (rvect.x + rvect.y != 0) // push away along reverse speed vector
				{
					rvect /= sqrt((rvect.x * rvect.x) + (rvect.y * rvect.y)); // normalize rvect
					rvect *= 0.45f; // we've got a little vector looking opposite direction of the spritex speed vector
					do // push spritex away from another one
					{
						(*i).second->setPosition((*i).second->getPosition() - rvect);
					} while (_findCollision((*i).second, NULL));
				};
				// If the ball hit object, apply explosion to the object and change ball's direction
				if ((*i).second == getSpritex(_ballID))
				{
					_ballHitSound.play();
					_applyExplosion(&collisionData);
					// Calculate reflected velocity vector
					curPos = ((*i).second)->getPosition();
					vel = ((*i).second)->getSpeed();
					curPos += sf::Vector2f((float)((*i).second)->getSize().x / 2, (float)((*i).second)->getSize().y / 2); // curPos = center of sprite
					// rvect is a normal to the tangent line to the ball passing through collision point (CO vector, where C is a collision point, O is a center of spritex)
					rvect = collisionData.collisionPoint - curPos;
					rvect /= sqrt((rvect.x * rvect.x) + (rvect.y * rvect.y)); // normalize rvect
					// Simply revert one coordinate depending on rvect direction
					#define COS45 0.70710678118654752440084436210485f
					#define COS135 -0.70710678118654752440084436210485f
					// 
					// 1 
					if (rvect.x > COS45 && rvect.y > COS135 && rvect.y < COS45)
					{
						vel.x = -vel.x;
					};
					// 2
					if (rvect.x < COS135 && rvect.y > COS135 && rvect.y < COS45)
					{
						vel.x = -vel.x;
					};
					// 3
					if(rvect.x > COS135 && rvect.x < COS45 && rvect.y < COS135)
					{
						vel.y = -vel.y;
					};
					// 4
					if (rvect.x > COS135 && rvect.x < COS45 && rvect.y > COS45)
					{
						vel.y = -vel.y;
					};
					(*i).second->setSpeed(vel);
				};
				// Special events for diamon collision
				if ((*i).second->isDiamond())
				{
					(*i).second->setSpeed(sf::Vector2f(0, 0)); // Stop diamond falling
					if (collisionData.collisionee == getSpritex(_paddleID)) _harvestDiamond((*i).second); // Apply diamond harvesting, if diamond hit paddle
				};
				// Special events for paddle collision
				if ((*i).second == getSpritex(_paddleID))
				{
					if (collisionData.collisionee->isDiamond()) _harvestDiamond(collisionData.collisionee);
				};
			};
		};
		// If diamond was not picked up by the player and it was gone, then harvest it anyway
		if (((*i).second)->isDiamond())
		{
			if (_diamondIsOutOfScreen((*(*i).second))) _harvestDiamond((*i).second);
			// Player won
			if (_diamondsGained == _numDiamonds)
			{
				//isRunning = false;
				++_currentLevel;
				if (!loadLevelData(_currentLevel)) isRunning = false;
				_diamondsGained = 0;
				_numLives = LIVES_MAX;
				return;
			};
		};
		// Remove dead spritexes
		if (((*i).second)->isDead())
		{
			delete (*i).second;
		    _spritexes.erase(i++);
		}
		else
		{
			i++;
		};
	}; // for (SpritexMapIterator i = _spritexes.begin(); i != _spritexes.end();)
	if (_ballIsOutOfScreen())
	{
		_numLives--;
		if (_numLives == 0)
		{
			isRunning = false;
		}
		else
		{
			_isBallGluedToPaddle = true;
		};
	};
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Board::_findCollision(Spritex* s, CollisionData* collisionData)
{
	Spritex* d;
	for (SpritexMapIterator i = _spritexes.begin(); i != _spritexes.end(); i++)
    {
		d = (*i).second;
		if (s == d) continue; // skip self
		sf::Vector2f tmpV;
		if (s->collides(*d, true, false, &tmpV))
		{
			if (collisionData != 0)
			{
				collisionData->collisionPoint = tmpV;
				collisionData->collisionee = d;
			};
			return true;
		};
	};
	return false;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Board::_ballIsOutOfScreen()
{
	Spritex* ball = getSpritex(_ballID);
	sf::FloatRect ballRect = ball->getTransform().transformRect(ball->getAABB());
	sf::FloatRect screenRect(0, 0, RESOLUTION_X, RESOLUTION_Y);
	return !screenRect.intersects(ballRect);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Board::_diamondIsOutOfScreen(const Spritex& s)
{
	if (!s.isDiamond()) return false;
	sf::FloatRect diamondRect = s.getTransform().transformRect(s.getAABB());
	sf::FloatRect screenRect(0, 0, RESOLUTION_X, RESOLUTION_Y);
	return !screenRect.intersects(diamondRect);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Board::_applyExplosion(CollisionData* collisionData)
{
	if (!collisionData->collisionee->isDestructible()) return;

	sf::Vector2f cur, cp;
	double distortion, distance;
	uint8_t density;

	_explodeSound.play();
	srand((unsigned)time(NULL));
	cp = collisionData->collisionPoint; // collisionPoint contains global coordinates of last collision point
	cp = collisionData->collisionee->getInverseTransform().transformPoint(cp); // transform 'cp' to local 'collisionee' coordinates
	// Find explosion boundaries
	int miny = cp.y - EXPLOSION_RADIUS > 0 ? static_cast<int>(cp.y - EXPLOSION_RADIUS) : 0;
	int maxy = cp.y + EXPLOSION_RADIUS < collisionData->collisionee->getSize().y ? static_cast<int>(cp.y + EXPLOSION_RADIUS) : static_cast<int>(collisionData->collisionee->getSize().y);
	int minx = cp.x - EXPLOSION_RADIUS > 0 ? static_cast<int>(cp.x - EXPLOSION_RADIUS) : 0;
	int maxx = cp.x + EXPLOSION_RADIUS < collisionData->collisionee->getSize().x ? static_cast<int>(cp.x + EXPLOSION_RADIUS) : static_cast<int>(collisionData->collisionee->getSize().x);
	// Apply explosion per pixel
	for (int y = miny; y < maxy; y++)
		for (int x = minx; x < maxx; x++)
		{
			distance = sqrt((x - cp.x) * (x - cp.x) + ((y - cp.y) * (y - cp.y)));
			//distortion = ((double)rand() / (RAND_MAX + 1) * EXPLOSION_MAX_DISTORTION);
			distortion = 0;
			if (distance <= EXPLOSION_RADIUS/2 + distortion)
			{
				density = collisionData->collisionee->getDensityAt(x, y);
				if (density == 0) continue; // pixel is already empty
				--density;
				if (density == 0) // destroy pixel
				{
					collisionData->collisionee->setPixel(x, y, sf::Color::Transparent);
					collisionData->collisionee->setDensityAt(x, y, 0, 0);
				}
				else // update pixel density
				{
				// TODO: ball sticks in the collisionee when this enabled. Need fix
				//	collisionData->collisionee->setDensityAt(x, y, density, 255);
				};
			};
		};
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Board::_harvestDiamond(Spritex* diamond)
{
	_harvestSound.play();
	_diamondsGained++;
	removeSpritex(diamond);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
sf::Vector2f Board::_deviateVectorToRandomAngle(sf::Vector2f& v, float maxAngle)
{
	std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, 1000);
	double rotationDirection = (dist(rng) % 2) == 0 ? -1.0 : +1.0;
	double k = dist(rng) / 1000.0;
	double cosa = cos(maxAngle * k * rotationDirection);
	double sina = sin(maxAngle * k * rotationDirection);
	float vlen = sqrt(v.x * v.x + v.y * v.y);
	sf::Vector2f dvel;
	dvel.x = static_cast<float>(v.x * cosa - v.y * sina);
	dvel.y = static_cast<float>(v.x * sina + v.y * cosa);
	dvel = (dvel / sqrt(dvel.x * dvel.x + dvel.y * dvel.y)) * vlen;
	return dvel;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Board::run(sf::RenderWindow &gameWindow)
{
	sf::Event Event;
	sf::Clock updateClock;
	int lastUpdateTimeMSec = 0;

	isRunning = true;
	isPaused = false;
	loadLevelData(_currentLevel);
	updateClock.restart();
	resetClock();
	_music.setVolume(20); _music.setLoop(true); _music.play();
	while (isRunning)
    {
		// Handle events
        while (gameWindow.pollEvent(Event))
        {
			switch(Event.type)
			{
				// Window closed, shutdown game
				case sf::Event::Closed:
					isRunning = false;
					break;
				case sf::Event::KeyPressed:
					switch (Event.key.code)
					{
						// Escape key pressed, shutdown game
						case sf::Keyboard::Escape:
							isRunning = false;
							_music.stop();
							break;
						// P key pressed, pause game
						case sf::Keyboard::P:
							isPaused = ! isPaused;
							resetClock();
							break;
						//
						// Some debug cheats
						//
						case sf::Keyboard::Add:
							_diamondsGained = _numDiamonds;
							break;
						case sf::Keyboard::Subtract:
							// Further it'll be incremented and we got to _currentLevel-1 level actually
							_currentLevel -= 2;
							_diamondsGained = _numDiamonds;
							break;
					};
					break;
				case sf::Event::MouseButtonPressed:
					break;
			};
		};
		// Handle input
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			setPaddleSpeed(sf::Vector2f(-PADDLE_SPEED, 0));
		} else
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			{
				setPaddleSpeed(sf::Vector2f(+PADDLE_SPEED, 0));
			}
			else
				setPaddleSpeed(sf::Vector2f(0, 0));
		};
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && _isBallGluedToPaddle)
		{
			_isBallGluedToPaddle = false;
			//sf::Vector2f bs = _deviateVectorToRandomAngle(sf::Vector2f(BALL_SPEED_X, BALL_SPEED_Y), 30.0f * 3.14159265f / 180.0f);
			sf::Vector2f bs = _deviateVectorToRandomAngle(sf::Vector2f(BALL_SPEED_X, BALL_SPEED_Y), 1);
			setBallSpeed(bs);
		};

		gameWindow.clear();
		if (!isPaused && (updateClock.getElapsedTime().asMilliseconds() - lastUpdateTimeMSec) > UPDATE_PERIOD_MSEC)
		{
			if (_isBallGluedToPaddle)
			{
				sf::Vector2f paddlePos = getSpritex(_paddleID)->getPosition();
				getSpritex(_ballID)->setPosition(paddlePos.x + (PADDLE_WIDTH - BALL_SIZE)/2, paddlePos.y - BALL_SIZE);
			}
			processSpritexes();
			lastUpdateTimeMSec = updateClock.getElapsedTime().asMilliseconds();
		};
		drawBoard(gameWindow);
		_sNumGems.setString(boost::lexical_cast<std::string>((int)(_diamondsGained)) + "/" + boost::lexical_cast<std::string>((int)(_numDiamonds)));
		_sNumLives.setString(boost::lexical_cast<std::string>((int)(_numLives)));
		_sLevelInfo.setString(_levelInfo);
		gameWindow.draw(_sNumGems);
		gameWindow.draw(_sNumLives);
		gameWindow.draw(_sLevelInfo);
		if (isPaused)
		{
			sf::Text pausedText("Paused", _font, PAUSED_FONT_SIZE);
			sf::FloatRect fr = pausedText.getGlobalBounds();
			pausedText.setPosition((RESOLUTION_X - fr.width) / 2, RESOLUTION_Y - PAUSED_FONT_SIZE * 1.5);
			gameWindow.draw(pausedText);
		};
		gameWindow.display();
		// Calculate FPS
    };
};

bool Board::loadLevelData(int levelNum)
{
	using namespace rapidjson;
	Document d;
	//libconfig::Config levels;
	std::string levelCode, levelImage, levelDensity;
	unsigned int tmpID, gemCount, gx, gy, gemIdx;

	// Open data file
	std::ifstream levelsFile("data/levels.json", std::ifstream::in);
	std::string s((std::istreambuf_iterator<char>(levelsFile)), std::istreambuf_iterator<char>());
	levelsFile.close();
	ParseResult pr = d.Parse(s.c_str());
	if (!pr) {
		return false;
	}
	if (!d.IsArray()) return false;
	//levels.readFile("data/levels.dat");
	// Clear old level data and reload base resources
	_clearSpritexes();
	loadResources();
	// Read level
	////const libconfig::Setting& root = levels.getRoot(); 
	try
	{
		////const libconfig::Setting& levels = root["levels"];
		////int levelCount = levels.getLength();
		int levelCount = d.Capacity();
		if (levelNum <= 0 || levelNum > levelCount) return false;
		levelCode = d[levelNum-1]["code"].GetString();
		levelImage = d[levelNum-1]["image"].GetString();
		levelDensity = d[levelNum-1]["density"].GetString();
		////const libconfig::Setting& level = levels[levelNum - 1];
		////level.lookupValue("code", levelCode);
		////level.lookupValue("image", levelImage);
		////level.lookupValue("density", levelDensity);
		tmpID = addSpritex(new Diamondek::Spritex(levelImage, levelDensity));
		getSpritex(tmpID)->isDynamic(false);
		getSpritex(tmpID)->isDestructible(true);
		getSpritex(tmpID)->isDiamond(false);
		gemCount = d[levelNum - 1]["gems"].Capacity();
		////const libconfig::Setting& gems = level["gems"];
		////gemCount = gems.getLength();
		for (unsigned int gc = 0; gc < gemCount; gc++)
		{
			gx = d[levelNum - 1]["gems"][gc]["x"].GetInt();
			gy = d[levelNum - 1]["gems"][gc]["y"].GetInt();
			gemIdx = d[levelNum - 1]["gems"][gc]["idx"].GetInt();
			////gems[gc].lookupValue("x", gx);
			////gems[gc].lookupValue("y", gy);
			////gems[gc].lookupValue("idx", gemIdx);
			tmpID = addSpritex(new Diamondek::Spritex(std::string("data/gem").append(boost::lexical_cast<std::string>(gemIdx)).append(".png")));
			getSpritex(tmpID)->isDynamic(true);
			getSpritex(tmpID)->isDestructible(false);
			getSpritex(tmpID)->isDiamond(true);
			getSpritex(tmpID)->setPosition(static_cast<float>(gx), static_cast<float>(gy));
			getSpritex(tmpID)->applyForce(sf::Vector2f(0, G_ACCELERATION));
			removeCollidingBackground(getSpritex(tmpID));
		};
		setNumberOfDiamonds(gemCount);
		_levelInfo = "Level " + boost::lexical_cast<std::string>((int)(_currentLevel)) + ", code: " + levelCode;
	}
	catch(...)
	{
		return false;
	};
	return true;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Board::loadResources()
{
	uint32_t tmpID;

	// Load board
	tmpID = addSpritex(new Diamondek::Spritex("data/board.png", "data/board_density.png"));
	getSpritex(tmpID)->isDynamic(false);
	getSpritex(tmpID)->isDestructible(false);
	getSpritex(tmpID)->isDiamond(false);
	// Load ball
	tmpID = addSpritex(new Diamondek::Spritex("data/ball.png"));
	getSpritex(tmpID)->isDynamic(true);
	getSpritex(tmpID)->isDestructible(false);
	getSpritex(tmpID)->isDiamond(false);
	getSpritex(tmpID)->setPosition(PADDLE_POS_X + PADDLE_WIDTH/2, PADDLE_POS_Y - PADDLE_HEIGHT);
	setBallID(tmpID);
	_isBallGluedToPaddle = true;
	// Load paddle
	tmpID = addSpritex(new Diamondek::Spritex("data/paddle.png"));
	getSpritex(tmpID)->isDynamic(true);
	getSpritex(tmpID)->isDestructible(false);
	getSpritex(tmpID)->isDiamond(false);
	getSpritex(tmpID)->setPosition(PADDLE_POS_X, PADDLE_POS_Y);
	setPaddleID(tmpID);
};

}; // namespace Diamondek