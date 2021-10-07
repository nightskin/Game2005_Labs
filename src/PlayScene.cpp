#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"

// required for IMGUI
#include "imgui.h"
#include "imgui_sdl.h"
#include "Renderer.h"
#include "Util.h"

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
			nextPoint.y = startPos.y + vy * timeSinceLaunch + 0.5 * gravity * pow(timeSinceLaunch, 2);

			//Draw Line
			SDL_RenderDrawLine(Renderer::Instance().getRenderer(), lastPoint.x, lastPoint.y, nextPoint.x, nextPoint.y);
			//Increment time
			timeSinceLaunch += timeStep;
			//Update Points
			lastPoint = nextPoint;
		}
	}


	


	SDL_Rect *intecerpt1 = new SDL_Rect();
	intecerpt1->h = 10;
	intecerpt1->w = 10;
	intecerpt1->y = groundLv - intecerpt1->h/2;
	intecerpt1->x = lastPoint.x - intecerpt1->w * 2;


	//Draw Ground as black line
	SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 0, 0, 0, 255);
	SDL_RenderDrawLine(Renderer::Instance().getRenderer(), 0, groundLv, 800, groundLv);

	//Draw Intercepts as blue sqaures
	SDL_SetRenderDrawColor(Renderer::Instance().getRenderer(), 0, 0, 255, 255);
	SDL_RenderFillRect(Renderer::Instance().getRenderer(), intecerpt1);

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
		acceleration.y += gravity * dt;
		velocity += acceleration * dt;
		//Move Grenade
		grenade->getTransform()->position += velocity;
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
	
	ImGui::SliderFloat("LaunchElevationAngle", &launchElevationAngle, -90.0f, 90.0f, "%.3f");
	ImGui::SliderFloat("launchSpeed", &launchSpeed, -500.0f, 500.0f, "%.3f");
	ImGui::SliderFloat("accelerationGravity", &gravity, -1500.0f, 1500.0f, "%.3f");
	ImGui::SliderFloat2("StartPosY", &startPos.x, -0.0f, 1000.0f, "%.1f,");

	ImGui::End();
}
