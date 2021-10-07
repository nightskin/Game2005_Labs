#pragma once

#include "TextureManager.h"
#include <glm/vec4.hpp>

#include "Agent.h"

class Bomb final : public Agent
{
public:
	Bomb();
	~Bomb();

	void draw() override;
	void update() override;
	void clean() override;
private:

};
