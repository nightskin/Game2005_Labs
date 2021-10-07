#pragma once
#ifndef __PLAYER__
#define __PLAYER__

#include "PlayerAnimationState.h"
#include "Sprite.h"

class Player final : public Sprite
{
public:
	Player();
	~Player();

	// Life Cycle Methods
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;

	// setters
	void setAnimationState(PlayerAnimationState new_state);
	void Move(float x, float y);
	void Move(glm::vec2 direction);
private:
	void m_buildAnimations();
	PlayerAnimationState m_currentAnimationState;
};

#endif /* defined (__PLAYER__) */