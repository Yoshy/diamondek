/*! 
	\class Diamondek::Spritex
    \brief Spritex class

    Spritex (sprite extended) is a sprite with some additional functionality (image "density" implementation in our case).
*/

#ifndef _SPRITEX_H_
#define _SPRITEX_H_

#include <SFML/Graphics.hpp>

#define MAX_DENSITY_DEFAULT 1
#define SPEED_POW2_THRESHOLD 0.0025f

namespace Diamondek {

class Spritex;

typedef std::map<uint32_t, Spritex*> SpritexMap;
typedef SpritexMap::iterator SpritexMapIterator;

class Spritex : public sf::Drawable, public sf::Transformable
{
public:
	/// Construct spritex from image file.
	/// Density calculated from rgb-values of pixels, more darken pixels are more though
	Spritex(const std::string& filename, unsigned int maxDensity = MAX_DENSITY_DEFAULT);
	/// Construct spritex from pixelmap and densitymap files
	Spritex(const std::string& pixelmap, const std::string& densitymap);
	~Spritex(void);
	//
	// Basic visual & density manipulations
	//
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	const sf::Vector2f getSize() const { return sf::Vector2f(_sprite.getLocalBounds().width, _sprite.getLocalBounds().height); };
	const sf::FloatRect getAABB() const { return _sprite.getGlobalBounds(); };
	int getDensityAt(int x, int y) { return _densityMap.getPixel(x, y).r; };
	void setDensityAt(int x, int y, int density, int alpha) { _densityMap.setPixel(x, y, sf::Color(density, density, density, alpha)); };
	void setPixel(int x, int y, sf::Color c) { unsigned char pixel[4] = {c.r, c.g, c.b, c.a}; _texture.update(pixel, 1, 1, x, y); };
	//
	// Trivial physics
	// It's assumed that a physic tick has fixed dt
	//
	/// Apply permanent force to spritex
	void applyForce(sf::Vector2f force) { _accel += force; };
	/// Apply force only for one tick
	void applyImpulseOfForce(sf::Vector2f force);
	/// Calculate and move spritex to the new position according to current speed and applied forces
	void physicsTick();
	void setSpeed(const sf::Vector2f& speed) { if (_dynamic) _speed = speed; };
	const sf::Vector2f& getSpeed() const { return _speed; };
	//
	// Collision detection
	//
	/// Return true, if this Spritex collides with given 'second' Spritex.
	/// If 'pp' is true, then method uses pixel perfect test for collision detection, otherwise is uses AABB intersection test
	/// If 'remove' is true, then colliding pixels of second spritex are removed to eliminate collision. Note, that 'pp' must be true for remove to work
	/// Warning! Because of the fact, that in the pair of given spritexes actually works method of a smaller spritex, remove will always affect a larger one
	bool collides(Spritex& second, bool pp, bool remove, sf::Vector2f* collisionPoint);
	//
	// Actually, these methods are out of place and must be in the other class
	//
	void setDead() { _dead = true; };
	bool isDead() const { return _dead; };
	bool isDynamic() const { return _dynamic; };
	bool isDynamic(bool v) { _dynamic = v; return _dynamic; };
	bool isDestructible() const { return _destructible; };
	bool isDestructible(bool v) { _destructible = v; return _destructible; };
	bool isDiamond() const { return _diamond; };
	bool isDiamond(bool v) { _diamond = v; return _diamond; };
	//
	// For debug purposes
	//
	/// This function will draw density map and bounding box at specified coordinates
	void dbgDrawDensityMap(sf::RenderTarget& target, sf::Vector2f position);
	/// This function will draw alpha channel map and bounding box at specified coordinates
	void dbgDrawAlphaMap(sf::RenderTarget& target, sf::Vector2f position);

private:
	/// Spritex speed
	sf::Vector2f _speed;
	/// Spritex acceleration (sum of applied forces)
	sf::Vector2f _accel;
	/// This bitmap is used for collision detection and "density" operations
	/// Alpha channel of this image is used for collision detection. 0 - transparent pixel (no collision), any other value collides
	/// R = G = B and are used for "density"
	sf::Image _densityMap;
	/// Sprite contain texture correcponding to '_texture' and other 'Sprite' stuff
	sf::Sprite _sprite;
	/// This texture object contain main texture of the 'Spritex'
	sf::Texture _texture;
	/// This texture object is used for 'dbgDrawAlphaMap' method
	sf::Texture _dbgAlphaTexture;
	/// return true if AABBs of this and that spritexes are intersected
	bool _AABBIntersection(const Spritex& second);
	/// Types of spritexes:
	/// DYNAMIC: spritex can move, and therefore must be checked for collisions with other spritexes
	/// STATIC: spritex doesn't move, so it's not needed to be checked for collisions
	/// DESTRUCTABLE spritexes can be destroyed
	/// DIAMOND: diamond object disappears, when collided with paddle, thus increasing paddle energy 
	bool _destructible;
	bool _dynamic;
	bool _diamond;
	bool _dead;
	//
	// Utility methods
	//
	void _initDefaults();
	void _drawTextureAndAABB(sf::RenderTarget& target, const sf::Vector2f& position, const sf::Texture& t);
	/// Draws AABB of transformed _densityMap
	void _drawAABB(sf::RenderTarget& target, const sf::Vector2f& position);
	/// Prepare _dbgAlphaTexture for use
	void _prepareDbgAlphaTexture();
	/// Return true if value 'v' is between 'min' and 'max'
	//bool _between(float v, float min, float max) { return (min < v) && (v < max); };
};

}; // namespace Diamondek

#endif // _SPRITEX_H_
