#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"

//Required for IMGUI
#include "imgui.h"
#include "imgui_sdl.h"
#include "Renderer.h"
#include "Util.h"

//Other Stuff
#include <string>

PlayScene::PlayScene()
{
	PlayScene::start();
}

PlayScene::~PlayScene()
= default;

void PlayScene::draw()
{
	drawDisplayList();

	SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 255, 0, 0, 255);

	float vx = cos(launchElevationAngle * M_PI / 180) * launchSpeed;
	float vy = sin(-launchElevationAngle * M_PI / 180) * launchSpeed;
	SDL_RenderDrawLine(Renderer::Instance().getRenderer(), startPos.x, startPos.y, startPos.x + vx, startPos.y + vy);
	


	float totalSimSec = 20.0f;
	int numPoints = 180;
	float timeStep = totalSimSec / (float)numPoints;
	float timeSinceLaunch = 0.0f; //Time Since launch
	glm::vec2 lastPoint = startPos;
	glm::vec2 nextPoint;
	
	SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 0, 200, 0, 255);
	//Kinematic equation for motion of bomb
	for (int i = 0; i < numPoints; i++)
	{
		if (nextPoint.y < groundLv)
		{
			nextPoint.x = startPos.x + vx * timeSinceLaunch;
			nextPoint.y = startPos.y + vy * timeSinceLaunch + 0.5 * gravity* pow(timeSinceLaunch, 2);

			//Draw Line
			SDL_RenderDrawLine(Renderer::Instance().getRenderer(), lastPoint.x, lastPoint.y, nextPoint.x, nextPoint.y);
			//Increment time
			timeSinceLaunch += timeStep;
			//Update Points
			lastPoint = nextPoint;
		}
	}


	


	SDL_Rect *intecerpt = new SDL_Rect();
	intecerpt->h = 10;
	intecerpt->w = 10;
	intecerpt->y = groundLv - intecerpt->h/2;
	intecerpt->x = lastPoint.x - intecerpt->w * 2;


	//Draw Ground as black line
	SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 0, 0, 0, 255);
	SDL_RenderDrawLine(Renderer::Instance().getRenderer(), 0, groundLv, 800, groundLv);

	//Draw Intercepts as black sqaures
	SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 0, 0, 0, 255);
	SDL_RenderFillRect(Renderer::Instance().getRenderer(), intecerpt);

	
	float s = 15;

	//Draw Velocity as red line
	glm::vec2 v = glm::vec2(grenade->getTransform()->position.x + velocity.x * s, grenade->getTransform()->position.y - -velocity.y * s);
	SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 255, 0, 0, 255);
	SDL_RenderDrawLine(Renderer::Instance().getRenderer(), grenade->getTransform()->position.x, grenade->getTransform()->position.y, v.x,v.y);

	//Draw Acceleration as blue line
	glm::vec2 a = glm::vec2(grenade->getTransform()->position.x + acceleration.x * s, grenade->getTransform()->position.y - -acceleration.y * s);
	SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 0, 0, 255, 255);
	SDL_RenderDrawLine(Renderer::Instance().getRenderer(), grenade->getTransform()->position.x, grenade->getTransform()->position.y, a.x, a.y);

	SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 255, 255, 255, 255);
}

void PlayScene::update()
{
	
	updateDisplayList();
	float dt = Game::Instance().getDeltaTime();
	velocity.x = cos(launchElevationAngle * M_PI / 180) * launchSpeed * dt;
	velocity.y = sin(-launchElevationAngle * M_PI / 180) * launchSpeed * dt;

	if (simStarted)
	{
		acceleration.y += gravity* dt;
		velocity += acceleration * dt;
		//Move Grenade
		if (grenade->getTransform()->position.y <= groundLv - 32)
		{
			grenade->getTransform()->position += velocity;
		}
		else
		{
			velocity.y = 0;
			acceleration.y = 0;
			grenade->getTransform()->position.x += velocity.x;
		}

	}
	else
	{
		grenade->getTransform()->position = glm::vec2(startPos.x, startPos.y);
		velocity = glm::vec2(0, 0);
		acceleration = glm::vec2(0, 0);
		
	}


}

void PlayScene::clean()
{
	removeAllChildren();
}

void PlayScene::handleEvents()
{
	EventManager::Instance().update();

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_SPACE))
	{
		if (!spacePressed)
		{
			if (simStarted)
			{
				grenade->getTransform()->position = glm::vec2(startPos.x, startPos.y);
				simStarted = false;
			}
			else
			{
				simStarted = true;
			}
		}
		spacePressed = true;
	}
	else
	{
		spacePressed = false;
	}
	
	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_ESCAPE))
	{
		TheGame::Instance().quit();
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_1))
	{
		TheGame::Instance().changeSceneState(START_SCENE);
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_2))
	{
		TheGame::Instance().changeSceneState(END_SCENE);
	}
}

void PlayScene::start()
{
	// Set GUI Title
	m_guiTitle = "Play Scene";
	
	// Projectile Sprite
	grenade = new Bomb();
	addChild(grenade);
	grenade->getTransform()->position = glm::vec2(startPos.x,startPos.y);


	// Back Button
	m_pBackButton = new Button("../Assets/textures/backButton.png", "backButton", BACK_BUTTON);
	m_pBackButton->getTransform()->position = glm::vec2(300.0f, 400.0f);
	m_pBackButton->addEventListener(CLICK, [&]()-> void
	{
		m_pBackButton->setActive(false);
		TheGame::Instance().changeSceneState(START_SCENE);
	});

	m_pBackButton->addEventListener(MOUSE_OVER, [&]()->void
	{
		m_pBackButton->setAlpha(128);
	});

	m_pBackButton->addEventListener(MOUSE_OUT, [&]()->void
	{
		m_pBackButton->setAlpha(255);
	});
	addChild(m_pBackButton);

	// Next Button
	m_pNextButton = new Button("../Assets/textures/nextButton.png", "nextButton", NEXT_BUTTON);
	m_pNextButton->getTransform()->position = glm::vec2(500.0f, 400.0f);
	m_pNextButton->addEventListener(CLICK, [&]()-> void
	{
		m_pNextButton->setActive(false);
		TheGame::Instance().changeSceneState(END_SCENE);
	});

	m_pNextButton->addEventListener(MOUSE_OVER, [&]()->void
	{
		m_pNextButton->setAlpha(128);
	});

	m_pNextButton->addEventListener(MOUSE_OUT, [&]()->void
	{
		m_pNextButton->setAlpha(255);
	});

	addChild(m_pNextButton);

	/* Instructions Label */
	m_pInstructionsLabel = new Label("Press the backtick (`) character to toggle Debug View", "Consolas");
	m_pInstructionsLabel->getTransform()->position = glm::vec2(Config::SCREEN_WIDTH * 0.5f, 500.0f);

	addChild(m_pInstructionsLabel);

	ImGuiWindowFrame::Instance().setGUIFunction(std::bind(&PlayScene::GUI_Function, this));
}

void PlayScene::GUI_Function() 
{
	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();
	
	ImGui::Begin("Controls", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove);
	
	std::string xStr = std::to_string(grenade->getTransform()->position.x);
	std::string yStr = std::to_string(grenade->getTransform()->position.y);

	const char* y = yStr.c_str();
	const char* x = xStr.c_str();

	ImGui::LabelText("X Position", x);
	ImGui::LabelText("Y Postition", y);
	ImGui::SliderFloat("Mass", &mass, -90.0f, 90.0f, "%.3f");
	ImGui::SliderFloat("LaunchAngle", &launchElevationAngle, -90.0f, 90.0f, "%.3f");
	ImGui::SliderFloat("LaunchSpeed", &launchSpeed, -500.0f, 500.0f, "%.3f");
	ImGui::SliderFloat("Gravity", &gravity, -1500.0f, 1500.0f, "%.3f");
	ImGui::SliderFloat2("StartPosition", &startPos.x, -0.0f, 1000.0f, "%.1f,");

	ImGui::End();
}
