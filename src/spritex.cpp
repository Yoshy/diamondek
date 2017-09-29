/*! 
	\class Diamondek::Spritex
    \brief Spritex class
*/

#include "spritex.h"

namespace Diamondek {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Spritex::Spritex(const std::string& filename, unsigned int maxDensity)
{
	int sx, sy;
	sf::Color c;
	float density;

	if (_densityMap.loadFromFile(filename) == false) throw "Error loading image " + filename;
	_texture.loadFromImage(_densityMap);
	_sprite.setTexture(_texture);
	sx = _densityMap.getSize().x;
	sy = _densityMap.getSize().y;
	for (int y = 0; y < sy; y++)
		for (int x = 0; x < sx; x++)
		{
			c = _densityMap.getPixel(x, y);
			// calculate density value as a mean of (R,G,B) values
			//density = 255 - (static_cast<float>(c.r + c.g + c.b) / 3);
			// normalize density to maxDensity
			//density = (density * maxDensity) / 255;
			density = MAX_DENSITY_DEFAULT;
			// update density map
			_densityMap.setPixel(x, y, sf::Color(static_cast<unsigned int>(density), static_cast<unsigned int>(density), static_cast<unsigned int>(density), c.a));
		};
	_initDefaults();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Spritex::Spritex(const std::string& pixelmap, const std::string& densitymap)
{
	if (_texture.loadFromFile(pixelmap) == false) throw "Error loading image" + pixelmap;
	if (_densityMap.loadFromFile(densitymap) == false) throw "Error loading image" + densitymap;
	if ((_texture.getSize() != _densityMap.getSize())) throw "Wrong combination of pixel and density maps";
	_sprite.setTexture(_texture);
	_initDefaults();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Spritex::~Spritex(void)
{
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Spritex::_initDefaults()
{
	_prepareDbgAlphaTexture();
	_destructible = false;
	_dynamic = false;
	_dead = false;
	_speed = sf::Vector2f(0, 0);
	_accel = sf::Vector2f(0, 0);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Spritex::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform = getTransform();
	target.draw(_sprite, states);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Spritex::_AABBIntersection(const Spritex& second)
{
	sf::FloatRect thisBB = _sprite.getLocalBounds();
	sf::Transform thisTransform = getTransform();
	sf::FloatRect thatBB = second._sprite.getLocalBounds();
	sf::Transform thatTransform = second.getTransform();
	// Apply current transform to sprite BB and get AABBs
	thisBB = thisTransform.transformRect(thisBB);
	thatBB = thatTransform.transformRect(thatBB);
	if (thisBB.intersects(thatBB)) return true; else return false;
	// following is overkill for AABBs...
	//// Calculate X prjection of this sprite
	//float thisMinX = thisBB.left;
	//float thisMaxX = thisBB.left + thisBB.width;
	//float thatMinX = thatBB.left;
	//float thatMaxX = thatBB.left + thatBB.width;
	//// Calculate Y prjection of this sprite
	//float thisMinY = thisBB.top;
	//float thisMaxY = thisBB.top + thisBB.height;
	//float thatMinY = thatBB.top;
	//float thatMaxY = thatBB.top + thatBB.height;
	//// Check collision
	//// If some end of the projection of bounding box to an axis is between ends of the projection of other spritex bounding box, then there is penetration of projections on this axis.
	//// If there is penetrations on both axes, then there is penetration of boxes (2D).
	//if (_between(thisMinX, thatMinX, thatMaxX) ||
	//	_between(thisMaxX, thatMinX, thatMaxX) ||
	//	_between(thatMinX, thisMinX, thisMaxX) ||
	//	_between(thatMaxX, thisMinX, thisMaxX))
	//{  // intersection
	//	if (_between(thisMinY, thatMinY, thatMaxY) ||
	//		_between(thisMaxY, thatMinY, thatMaxY) ||
	//		_between(thatMinY, thisMinY, thisMaxY) ||
	//		_between(thatMaxY, thisMinY, thisMaxY))
	//	{  // intersection of BBs
	//		return true;
	//	};
	//};
	//return false;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Spritex::applyImpulseOfForce(sf::Vector2f force)
{
	_speed += force;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Spritex::physicsTick()
{
	_speed += _accel;
	setPosition(getPosition() + _speed);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Spritex::collides(Spritex& second, bool pp, bool remove, sf::Vector2f* collisionPoint)
{
	// If AABBs are not collided, then there is nothing to talk about
	if (!_AABBIntersection(second)) return false;
	// If PixelPerfect is not needed, then we are done
	if (!pp) return true;
	// Let's PP hardcore begins
	sf::Vector2f size = getSize();
	sf::Vector2f otherSize = second.getSize();
	if ((size.x * size.y) > (otherSize.x * otherSize.y))
	{ // second sprite is smaller, it is cheaper to call its 'collide' method
		return second.collides(*this, pp, remove, collisionPoint);
	};
	sf::Transform secondReverseTransform = second.getInverseTransform();
	sf::Transform secondTransform = second.getTransform();
	sf::Transform reverseTransform = getInverseTransform();
	sf::Transform transform = getTransform();
	int alpha, otherAlpha;
	sf::Vector2f curPoint;
	for (int y = 0; y < size.y; y++)
	{
		for (int x = 0; x < size.x; x++)
		{
			alpha = _densityMap.getPixel(x, y).a;
			if (alpha == 0) continue; // skip transparent pixels
			curPoint = getTransform().transformPoint(sf::Vector2f(static_cast<float>(x), static_cast<float>(y))); // x,y to global coords
			curPoint = secondReverseTransform.transformPoint(curPoint); // from global coords to second local coords
			if ((curPoint.x < 0) || (curPoint.y < 0) || (curPoint.x > otherSize.x - 1) || (curPoint.y > otherSize.y - 1)) continue; // out of range
			otherAlpha = second._densityMap.getPixel(static_cast<int>(curPoint.x), static_cast<int>(curPoint.y)).a;
			if (otherAlpha != 0)
			{
				if (remove)
				{
					second.setPixel(static_cast<int>(curPoint.x), static_cast<int>(curPoint.y), sf::Color::Transparent);
					second._densityMap.setPixel(static_cast<int>(curPoint.x), static_cast<int>(curPoint.y), sf::Color::Transparent);
				}
				else
				{
					if (collisionPoint != NULL)
					{
						collisionPoint->x = static_cast<float>(x);
						collisionPoint->y = static_cast<float>(y);
						*collisionPoint = transform.transformPoint(*collisionPoint);
					};
					return true;
				};
			};
		};
	};
	if (remove) second._prepareDbgAlphaTexture();
	return false;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Spritex::dbgDrawDensityMap(sf::RenderTarget& target, sf::Vector2f position)
{
	sf::Texture tTexture;
	tTexture.loadFromImage(_densityMap);
	_drawTextureAndAABB(target, position, tTexture);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Spritex::dbgDrawAlphaMap(sf::RenderTarget& target, sf::Vector2f position)
{
	_drawTextureAndAABB(target, position, _dbgAlphaTexture);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Spritex::_drawTextureAndAABB(sf::RenderTarget& target, const sf::Vector2f& position, const sf::Texture& t)
{
	sf::Sprite tSpr(t);
	sf::RenderStates states;
	states.transform = getTransform();
	target.draw(tSpr, states);
	_drawAABB(target, position);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Spritex::_drawAABB(sf::RenderTarget& target, const sf::Vector2f& position)
{
	sf::VertexArray AABB;
	sf::FloatRect boundingBox = _sprite.getGlobalBounds();
	AABB.append(sf::Vertex(sf::Vector2f(boundingBox.left, boundingBox.top), sf::Color::Blue));
	AABB.append(sf::Vertex(sf::Vector2f(boundingBox.left + boundingBox.width, boundingBox.top), sf::Color::Blue));
	AABB.append(sf::Vertex(sf::Vector2f(boundingBox.left + boundingBox.width, boundingBox.top + boundingBox.height), sf::Color::Blue));
	AABB.append(sf::Vertex(sf::Vector2f(boundingBox.left, boundingBox.top + boundingBox.height), sf::Color::Blue));
	AABB.append(sf::Vertex(sf::Vector2f(boundingBox.left, boundingBox.top), sf::Color::Blue));
	sf::RenderStates states;
	states.transform = getTransform();
	target.draw(&AABB[0], 5, sf::LinesStrip, states);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Spritex::_prepareDbgAlphaTexture()
{
	// Represent alpha channel as grayscale image (r=g=b=(255-a))
	sf::Image tImage;
	sf::Color tColor;
	int sx = _densityMap.getSize().x;
	int sy = _densityMap.getSize().y;
	int c;
	tImage.create(sx, sy, sf::Color::Transparent);
	for (int y = 0; y < sy; y++)
		for (int x = 0; x < sx; x++)
		{
			tColor = _densityMap.getPixel(x, y);
			c = 255 - tColor.a; // More transparent pixel is more lighter
			tImage.setPixel(x, y, sf::Color(c, c, c));
		};
	_dbgAlphaTexture.loadFromImage(tImage);
};

}; // namespace Diamondek