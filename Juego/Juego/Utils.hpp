#ifndef UTILS_HPP
#define UTILS_HPP

#include <SFML/Graphics.hpp>
#include <unordered_map>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const sf::Vector2f WINDOW_RES(WINDOW_WIDTH, WINDOW_HEIGHT);

const sf::Vector2u TILESIZE(16,16);

const sf::Vector2f BUTTONSIZE(280, 60);
const sf::IntRect  BUTTONSELECTED1RECT(15, 125, BUTTONSIZE.x, BUTTONSIZE.y);
const sf::IntRect  BUTTONNORMAL1RECT(15, 203, BUTTONSIZE.x, BUTTONSIZE.y);
const sf::IntRect  BUTTONSELECTED2RECT(15, 281, BUTTONSIZE.x, BUTTONSIZE.y);
const sf::IntRect  BUTTON2NORMALRECT(15, 359, BUTTONSIZE.x, BUTTONSIZE.y);

const std::unordered_map<int, sf::IntRect> WALLTILE = {
	{ 0, sf::IntRect(32, 48, TILESIZE.x, TILESIZE.y)},
	{ 1, sf::IntRect(16, 48, TILESIZE.x, TILESIZE.y) },
	{ 2, sf::IntRect(32, 64, TILESIZE.x, TILESIZE.y) },
	{ 3, sf::IntRect(16, 64, TILESIZE.x, TILESIZE.y) },
	{ 4, sf::IntRect(0, 64, TILESIZE.x, TILESIZE.y) },
	{ 12, sf::IntRect(32, 64, TILESIZE.x, TILESIZE.y) },
	{ 13, sf::IntRect(0, 48, TILESIZE.x, TILESIZE.y) },
	{ 14, sf::IntRect(0, 64, TILESIZE.x, TILESIZE.y) },
	{ 23, sf::IntRect(32, 32, TILESIZE.x, TILESIZE.y) },
	{ 24, sf::IntRect(16, 32, TILESIZE.x, TILESIZE.y) },
	{ 34, sf::IntRect(0, 32, TILESIZE.x, TILESIZE.y) },
	{ 123, sf::IntRect(80, 48, TILESIZE.x, TILESIZE.y) },
	{ 124, sf::IntRect(64, 64, TILESIZE.x, TILESIZE.y) },
	{ 134, sf::IntRect(48, 48, TILESIZE.x, TILESIZE.y) },
	{ 234, sf::IntRect(64, 32, TILESIZE.x, TILESIZE.y) },
	{ 1234, sf::IntRect(64, 48, TILESIZE.x, TILESIZE.y) }
};

class TEXTURE_HOLDER{
	TEXTURE_HOLDER(){
		if (!t.loadFromFile("Textura.png")){
			exit(-1);
		}
	}



	sf::Texture t;
};

void centerOrigin(sf::Sprite& sprite);

void centerOrigin(sf::Text& text);

#endif