#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "Scene.h"
#include "Plane.h"
#include "Bomb.h"
#include "Player.h"
#include "Button.h"
#include "Label.h"

class PlayScene : public Scene
{
public:
	PlayScene();
	~PlayScene();

	// Scene LifeCycle Functions
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;
	virtual void handleEvents() override;
	virtual void start() override;
private:
	// IMGUI Function
	void GUI_Function();
	std::string m_guiTitle;
	
	glm::vec2 m_mousePosition;

	
	glm::vec2 startPos = glm::vec2(100,400);
	glm::vec2 velocity = glm::vec2(0,0);
	glm::vec2 acceleration = glm::vec2(0, 0);


	float launchElevationAngle = 45.0f;
	float launchSpeed = 300.0f;

	float groundLv = 550;
	float gravity = 981;
	float mass = 10;
	float friction = 0.42;

	Bomb* grenade;

	bool simStarted = false;
	bool spacePressed = false;

	// UI Items
	Button* m_pBackButton;
	Button* m_pNextButton;
	Label* m_pInstructionsLabel;
};

#endif /* defined (__PLAY_SCENE__) */