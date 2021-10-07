#include "Bomb.h"
#include "glm/gtx/string_cast.hpp"
#include "PlayScene.h"
#include "TextureManager.h"
#include "Util.h"

Bomb::Bomb()
{
	TextureManager::Instance().load("../Assets/textures/bomb.png", "bomb");

	auto size = TextureManager::Instance().getTextureSize("bomb");
	setWidth(size.x);
	setHeight(size.y);

	getTransform()->position = glm::vec2(400.0f, 300.0f);
	getRigidBody()->velocity = glm::vec2(0.0f, 0.0f);
	getRigidBody()->acceleration = glm::vec2(0.0f, 0.0f);
	getRigidBody()->isColliding = false;
	setType(AGENT);

	setCurrentHeading(0.0f);// current facing angle
	setCurrentDirection(glm::vec2(1.0f, 0.0f)); // facing right

	setLOSDistance(400.0f); // 5 ppf x 80 feet
	setLOSColour(glm::vec4(1, 0, 0, 1));
}

Bomb::~Bomb()
{
}

void Bomb::draw()
{
	// alias for x and y
	const auto x = getTransform()->position.x;
	const auto y = getTransform()->position.y;

	// draw the ship
	TextureManager::Instance().draw("bomb", x, y, getCurrentHeading(), 255, true);

}

void Bomb::update()
{

}

void Bomb::clean()
{
}
