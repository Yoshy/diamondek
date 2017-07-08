// (C) 2011 PowerBIT Studio
/*! 
	\class Diamondek::Board
    \brief Board class

    Game board - is where all action is done.
*/

#ifndef _BOARD_H_
#define _BOARD_H_

#include <SFML/Audio.hpp>
#include "globals.h"
#include "spritex.h"

namespace Diamondek {

class CollisionData {
public:
	sf::Vector2f collisionPoint;
	Diamondek::Spritex* collisionee;
};

class Board
{
public:
    explicit Board(const std::string& backgroundSpriteName);
	~Board();
	void loadResources();
	void setBallID(uint32_t ballID) { _ballID = ballID; };
	void setPaddleID(uint32_t paddleID) { _paddleID = paddleID; };
	void setNumberOfDiamonds(uint32_t n) { _numDiamonds = n; };
	/// Add spritex object to board and return its ID
	uint32_t addSpritex(Spritex* s);
	/// Remove spritex object from board
	void removeSpritex(Spritex* s);
	/// Return pointer to spritex object with specified id or null;
	Spritex* getSpritex(uint32_t id);
	/// Remove all colliding pixels of ALL spritexes (even undestructable), that collide with s. Really big power is there
	void removeCollidingBackground(Spritex* s);
	/// Draw spritexes
	void drawBoard(sf::RenderTarget& target);
	/// Manual speed control of the paddle
	void setPaddleSpeed(sf::Vector2f speed);
	/// Manual speed control of the ball
	void setBallSpeed(sf::Vector2f speed);
	/// Process movement, physics and collision detection of spritexes
	void processSpritexes();
	/// Reset internal game timer
	void resetClock() { _clock.restart(); };
	/// Main game loop
	void run(sf::RenderWindow &gameWindow);
	/// game states
	bool isPaused, isRunning;
private:
	/// Ball and paddle ID's
	uint32_t _paddleID, _ballID;
	/// Current level
	uint32_t _currentLevel;
	/// Current level information
	std::string _levelInfo;
	/// Current number of diamonds gained by the player. If _diamondsGained == _numDiamonds then level is completed
	uint32_t _diamondsGained;
	/// Total number of diamonds on the level
	uint32_t _numDiamonds;
	/// Number of lives
	uint32_t _numLives;
	/// True, если шар "приклеен" к ракетке и движетс€ вместе с ней
	bool _isBallGluedToPaddle;
	/// ѕрицепл€ет шарик к ракетке
	void _stickBallToPaddle();
	/// Return true, if ball is outside of visible screen area, else false
	bool _ballIsOutOfScreen();
	/// Return true, if given gem is outside of visible screen area, else false
	bool _diamondIsOutOfScreen(const Spritex& s);
	/// Collision detection of spritex 's'
	/// If 's' collide with other spritex, return true and collision point global coordinates with pointer to colliding object in collisionData,
	/// otherwise return false and collisionPoint remains unchanged.
	bool _findCollision(Spritex* s, CollisionData* collisionData);
	/// Explode radius of wall, with epicentre in collisionData.collisionPoint
	void _applyExplosion(CollisionData* collisionData);
	/// Remove diamond from scene and increase paddle energy
	void _harvestDiamond(Spritex* diamond);
	/// Deviate vector direction to random angle. Max deviation angle is 'maxAngle'[radians]
	sf::Vector2f _deviateVectorToRandomAngle(sf::Vector2f& v, float maxAngle);
	/// Clear _spritexes
	void Board::_clearSpritexes();
	/// Load levels number 'levelNum' data
	bool loadLevelData(int levelNum);

	/// Spritexes on the board
	SpritexMap _spritexes;
	uint32_t _nextID;
	sf::Clock _clock;
	
	/// Sound stuff
	sf::Music _music;
	sf::SoundBuffer _ballHitSoundBuffer;
	sf::Sound _ballHitSound;
	sf::SoundBuffer _explodeSoundBuffer;
	sf::Sound _explodeSound;
	sf::SoundBuffer _harvestSoundBuffer;
	sf::Sound _harvestSound;

	/// internal stuff
	sf::Font _font;
	sf::Texture _background;
	sf::Text _sNumLives;
	sf::Text _sNumGems;
	sf::Text _sLevelInfo;

};

}; // namespace Diamondek

#endif // _BOARD_H_
