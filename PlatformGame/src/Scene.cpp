#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Log.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Player.h"
#include "Map.h"
#include "Item.h"
#include "Enemy.h"
#include "Physics.h"
#include "CheckPointBF.h"
#include "GuiControl.h"
#include "GuiManager.h"

Scene::Scene() : Module()
{
	name = "scene";
	bg = nullptr;
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	//L04: TODO 3b: Instantiate the player using the entity manager
	player = (Player*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);
	player->SetParameters(configParameters.child("entities").child("player"));

	pugi::xml_node checkpointbf = configParameters.child("entities").child("checkpointbf");
	checkpoint = (CheckPointBF*)Engine::GetInstance().entityManager->CreateEntity(EntityType::CHECKPOINTBF);
	checkpoint->SetParameters(checkpointbf);
	checkpoint2 = (CheckPointBF*)Engine::GetInstance().entityManager->CreateEntity(EntityType::CHECKPOINTBF);
	checkpoint2->SetParameters(checkpointbf);
	checkpoint3 = (CheckPointBF*)Engine::GetInstance().entityManager->CreateEntity(EntityType::CHECKPOINTBF);
	checkpoint3->SetParameters(checkpointbf);

	// L16: TODO 2: Instantiate a new GuiControlButton in the Scene
	int scale = Engine::GetInstance().window.get()->GetScale();

	SDL_Rect btPos = { 400 , 300 , 120, 60 };
	guiBt = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "TEST", btPos, this);

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	CTtexture = Engine::GetInstance().textures->Load("Assets/Textures/CONTROLS.png");

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");

	changeLevel(sceneNode.child("entities").child("player").attribute("level").as_int(),
		sceneNode.child("entities").child("enemies").child("enemy").attribute("saved").as_bool());
	
	player->coins = sceneNode.child("entities").child("player").attribute("coins").as_int();
	player->lifes = sceneNode.child("entities").child("player").attribute("lifes").as_int();

	//enemies
	if (enemyList.size() > 0)
	{
		Vector2D enemyPos = Vector2D(sceneNode.child("entities").child("enemies").child("enemy").attribute("x").as_int(),
			sceneNode.child("entities").child("enemies").child("enemy").attribute("y").as_int());

		enemyList[0]->SetPosition(enemyPos);
	}

	takenItems.clear();

	checkpoint->CheckTaken = sceneNode.child("entities").child("checkpointbf").attribute("taken1").as_bool();
	checkpoint2->CheckTaken = sceneNode.child("entities").child("checkpointbf").attribute("taken2").as_bool();
	checkpoint3->CheckTaken = sceneNode.child("entities").child("checkpointbf").attribute("taken3").as_bool();

	// Create music
	menu_introMS = Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/menu_intro.wav", 0);

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	//L03 TODO 3: Make the camera movement independent of framerate
	float camSpeed = 1;

	Engine::GetInstance().render.get()->DrawTexture(bg, 0, 0);

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN and player->currentLevel < 6)
	{
		changeLevel(player->currentLevel + 1, true);
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN and player->currentLevel > 1)
	{
		changeLevel(player->currentLevel - 1, false);
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_H) == KEY_DOWN)
	{
		CTVisible = !CTVisible;
	}
	
	if ((enemyList.size() > 0 && enemyList[0]->position.getY() < -10) || (enemyList.size() > 0 && enemyList[0]->position.getY() > 340))
	{
		Engine::GetInstance().entityManager->DestroyEntity(enemyList[0]);
		enemyList.clear();
	}

	if (enemyList.size() > 0 && enemyList[0]->isDead == true)
	{
		Engine::GetInstance().entityManager->DestroyEntity(enemyList[0]);
		enemyList.clear();
	}

	if (enemyList.size() > 0 && player->checkDeath == true && player->isDead == false)
	{
		if (player->GetPosition().getY() + 7 > enemyList[0]->GetPosition().getY() && !player->invincible)
		{
			pugi::xml_document loadFile;
			pugi::xml_parse_result result = loadFile.load_file("config.xml");

			player->Die();
			player->checkDeath = false;

			player->loseLife();
			loadFile.child("config").child("scene").child("entities").child("player").attribute("lifes").set_value(player->lifes);
			loadFile.save_file("config.xml");

 			if (player->lifes == 0)
			{
				player->isDead = false;

				player->isFalling = false;
				player->isJumping = false;
				player->pbody->body->SetLinearVelocity(b2Vec2_zero);

				SpawnPoint();
				SaveState();
				checkpoint->CheckTaken = false;
			}
		}
		else
		{
			Engine::GetInstance().entityManager->DestroyEntity(enemyList[0]);
			enemyList.clear();

			player->JumpFX();
			player->KillFX();

			player->checkDeath = false;
		}
		if (!player->invincible)
		{
			player->pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -0.6), true);
		}
	}

	if (itemList.size() > 0 && player->takeItem == true)
	{
		//SONIDO PILLAR ITEM ABAJO, DEPENDIENDO DEL TIPO
		switch (itemList[0]->type)
		{
		case 1:
			//monedas
			player->addCoins(5);
			break;
		case 2:
			//poder invencible
			playerInvencibility.Start();
			playerInvincible = true;
			break;
		case 3:
			//vida
			player->lifes++;
			break;
		default:
			break;
		}

		Engine::GetInstance().entityManager->DestroyEntity(itemList[0]);
		itemList.clear();

		takenItems.push_back(player->currentLevel);
		player->takeItem = false;
	}

	if (playerInvincible)
	{
		if (playerInvencibility.ReadSec() < 5)
		{
			player->invincible = true;
		}
		else
		{
			player->invincible = false;
			playerInvincible = false;
		}
	}

	if (checkpoint->Saving == true)
	{
		SaveState();
		checkpoint->Saving = false;
	}
	if (checkpoint2->Saving == true)
	{
		SaveState();
		checkpoint2->Saving = false;
	}
	if (checkpoint3->Saving == true)
	{
		SaveState();
		checkpoint3->Saving = false;
	}

	if (player->Loading == true)
	{
		LoadState();
		player->Loading = false;
	}

	//Get mouse position and obtain the map coordinate
	int scale = Engine::GetInstance().window.get()->GetScale();
	Vector2D mousePos = Engine::GetInstance().input.get()->GetMousePosition();
	Vector2D mouseTile = Engine::GetInstance().map.get()->WorldToMap(mousePos.getX() - Engine::GetInstance().render.get()->camera.x / scale,
																	 mousePos.getY() - Engine::GetInstance().render.get()->camera.y / scale);

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
	{
		player->isDead = false;
		LoadState();
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		player->isDead = false;

		player->isFalling = false;
		player->isJumping = false;
		player->pbody->body->SetLinearVelocity(b2Vec2_zero);

		SpawnPoint();
		SaveState();
		checkpoint->CheckTaken = false;
		checkpoint2->CheckTaken = false;
		checkpoint3->CheckTaken = false;
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		player->isDead = false;

		player->isFalling = false;
		player->isJumping = false;
		player->pbody->body->SetLinearVelocity(b2Vec2_zero);

		SpawnPointLvl2();
		SaveState();
		checkpoint->CheckTaken = false;
		checkpoint2->CheckTaken = false;
		checkpoint3->CheckTaken = false;
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F5) == KEY_DOWN && player->isDead == false)
	{
		SaveState();
	}	

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_UP) == KEY_DOWN and checkpoint->onPlayer == true)
	{
		if (checkpoint2->CheckTaken)
		{
			changeLevel(4, true);
		}
		else if (checkpoint3->CheckTaken)
		{
			changeLevel(5, true);
		}
	}	
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_UP) == KEY_DOWN and checkpoint2->onPlayer == true)
	{
		if (checkpoint3->CheckTaken)
		{
			changeLevel(5, true);
		}
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN and checkpoint2->onPlayer == true)
	{
		if (checkpoint->CheckTaken)
		{
			changeLevel(3, true);
		}
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN and checkpoint3->onPlayer == true)
	{
		if (checkpoint2->CheckTaken)
		{
			changeLevel(4, true);
		}
		else if (checkpoint->CheckTaken)
		{
			changeLevel(3, true);
		}
	}
	
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F3) == KEY_DOWN && player->isDead == false)
	{
		if (enemyList.size() > 0)
		{
			changeLevel(player->currentLevel, true);
		}
		else
		{
			changeLevel(player->currentLevel, false);
		}

		Vector2D newPos = Vector2D(0, 0);
		
		player->isFalling = false;
		player->isJumping = false;
		player->pbody->body->SetLinearVelocity(b2Vec2_zero);

		switch (player->currentLevel)
		{
		case 1:
			newPos = Vector2D(251, 329);
			break;
		case 2:
			newPos = Vector2D(331, 269);
			break;
		case 3:
			newPos = Vector2D(221, 289);
			break;
		case 4:
			newPos = Vector2D(134, 309);
			break;
		case 5:
			newPos = Vector2D(134, 289);
			break;
		default:
			break;
		}

		player->SetPosition(newPos);
	}

	Vector2D newCheck = Vector2D(-50, 0);
	checkpoint->SetPosition(newCheck);
	checkpoint2->SetPosition(newCheck);
	checkpoint3->SetPosition(newCheck);

	switch (player->currentLevel)
	{
	case 3:
		newCheck = Vector2D(51, 207);
		checkpoint->SetPosition(newCheck);
		break;
	case 4:
		newCheck = Vector2D(51, 207);
		checkpoint2->SetPosition(newCheck);
		break;
	case 5:
		newCheck = Vector2D(51, 207);
		checkpoint3->SetPosition(newCheck);
	default:
		break;
	}

	if (CTVisible && CTtexture != nullptr)
	{
		int WT, HG;
		Engine::GetInstance().textures->GetSize(CTtexture, WT, HG);
		int windowWT, windowHG;
		Engine::GetInstance().window->GetWindowSize(windowWT, windowHG);

		SDL_Rect Juan = { windowWT - WT - 100, 0, WT * 1.5, HG * 1.5f };
		SDL_RenderCopy(Engine::GetInstance().render->renderer, CTtexture, nullptr, &Juan);
	}

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");
	//SDL_DestroyTexture(bg);
	if (CTtexture != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(CTtexture);
		CTtexture = nullptr;
	}

	return true;
}

// Return the player position
Vector2D Scene::GetPlayerPosition()
{
	return player->GetPosition();
}

// L15 TODO 1: Implement the Load function
void Scene::LoadState() {
	b2Vec2 Vdefault = b2Vec2(0, 0);

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");

	//Read XML and restore information

	//Player position
	Vector2D playerPos = Vector2D(sceneNode.child("entities").child("player").attribute("x").as_int(),
		sceneNode.child("entities").child("player").attribute("y").as_int());
	player->SetPosition(playerPos);
	player->pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, 0), true);
	takenItems.clear();

	changeLevel(sceneNode.child("entities").child("player").attribute("level").as_int(), 
		sceneNode.child("entities").child("enemies").child("enemy").attribute("saved").as_bool());

	player->coins = sceneNode.child("entities").child("player").attribute("coins").as_int();
	player->lifes = sceneNode.child("entities").child("player").attribute("lifes").as_int();

	checkpoint->CheckTaken = sceneNode.child("entities").child("checkpointbf").attribute("taken1").as_bool();
	checkpoint2->CheckTaken = sceneNode.child("entities").child("checkpointbf").attribute("taken2").as_bool();
	checkpoint3->CheckTaken = sceneNode.child("entities").child("checkpointbf").attribute("taken3").as_bool();

	player->invincible = false;
	playerInvincible = false;

	//enemies
	if (enemyList.size() > 0)
	{
		Vector2D enemyPos = Vector2D(sceneNode.child("entities").child("enemies").child("enemy").attribute("x").as_int(),
			sceneNode.child("entities").child("enemies").child("enemy").attribute("y").as_int());

		enemyList[0]->SetPosition(enemyPos);
	}

	player->pbody->body->SetLinearVelocity(Vdefault);
	player->splatted.Reset();
}

// L15 TODO 2: Implement the Save function
void Scene::SaveState() {

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");

	sceneNode.child("entities").child("player").attribute("lifes").set_value(player->lifes);
	sceneNode.child("entities").child("player").attribute("coins").set_value(player->coins);

	if (enemyList.size() > 0)
	{
		sceneNode.child("entities").child("enemies").child("enemy").attribute("x").set_value(enemyList[0]->GetPosition().getX());
		sceneNode.child("entities").child("enemies").child("enemy").attribute("y").set_value(enemyList[0]->GetPosition().getY());
		sceneNode.child("entities").child("enemies").child("enemy").attribute("saved") = true;
	}
	else
	{
		sceneNode.child("entities").child("enemies").child("enemy").attribute("saved") = false;
	}
	
	if (takenItems.size() > 0)
	{
		for (int i = 0; i < takenItems.size(); i++)
		{
			std::string lvl = "lvl" + std::to_string(takenItems[i]);
			sceneNode.child("entities").child("player").child("items").attribute(lvl.c_str()) = true;
		}
	}

	//Save info to XML 

	//Player position
	sceneNode.child("entities").child("player").attribute("x").set_value(player->GetPosition().getX());
	sceneNode.child("entities").child("player").attribute("y").set_value(player->GetPosition().getY());
	sceneNode.child("entities").child("player").attribute("level").set_value(player->currentLevel);

	sceneNode.child("entities").child("checkpointbf").attribute("taken1").set_value(checkpoint->CheckTaken);
	sceneNode.child("entities").child("checkpointbf").attribute("taken2").set_value(checkpoint2->CheckTaken);
	sceneNode.child("entities").child("checkpointbf").attribute("taken3").set_value(checkpoint3->CheckTaken);

	//Saves the modifications to the XML 
	loadFile.save_file("config.xml");
}

void Scene::SpawnPoint()
{
	Vector2D playerPos = Vector2D(251, 329);
	player->SetPosition(playerPos);

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");

	sceneNode.child("entities").child("player").attribute("level") = 1;
	sceneNode.child("entities").child("player").attribute("lifes") = 3;
	sceneNode.child("entities").child("player").attribute("coins") = 0;
	player->resetCoins();
	player->lifes = 3;

	player->invincible = false;
	playerInvincible = false;

	sceneNode.child("entities").child("player").child("items").attribute("lvl1").set_value(false);
	sceneNode.child("entities").child("player").child("items").attribute("lvl2").set_value(false);
	sceneNode.child("entities").child("player").child("items").attribute("lvl3").set_value(false);
	sceneNode.child("entities").child("player").child("items").attribute("lvl4").set_value(false);
	sceneNode.child("entities").child("player").child("items").attribute("lvl5").set_value(false);
	loadFile.save_file("config.xml");
	takenItems.clear();

	sceneNode.child("entities").child("checkpointbf").attribute("taken1").set_value(false);
	sceneNode.child("entities").child("checkpointbf").attribute("taken2").set_value(false);
	sceneNode.child("entities").child("checkpointbf").attribute("taken3").set_value(false);

	checkpoint->CheckTaken = false;
	checkpoint2->CheckTaken = false;
	checkpoint3->CheckTaken = false;

	changeLevel(1, true);
}

void Scene::SpawnPointLvl2()
{
	Vector2D playerPos = Vector2D(321, 279);
	player->SetPosition(playerPos);

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");

	sceneNode.child("entities").child("player").attribute("level").set_value(2);
	sceneNode.child("entities").child("player").attribute("lifes").set_value(3);
	sceneNode.child("entities").child("player").attribute("coins").set_value(0);

	player->invincible = false;
	playerInvincible = false;

	sceneNode.child("entities").child("player").child("items").attribute("lvl1").set_value(false);
	sceneNode.child("entities").child("player").child("items").attribute("lvl2").set_value(false);
	sceneNode.child("entities").child("player").child("items").attribute("lvl3").set_value(false);
	sceneNode.child("entities").child("player").child("items").attribute("lvl4").set_value(false);
	sceneNode.child("entities").child("player").child("items").attribute("lvl5").set_value(false);
	loadFile.save_file("config.xml");
	takenItems.clear();

	sceneNode.child("entities").child("checkpointbf").attribute("taken1").set_value(false);
	sceneNode.child("entities").child("checkpointbf").attribute("taken2").set_value(false);
	sceneNode.child("entities").child("checkpointbf").attribute("taken3").set_value(false);

	checkpoint->CheckTaken = false;
	checkpoint2->CheckTaken = false;
	checkpoint3->CheckTaken = false;

	changeLevel(2, true);
}

void Scene::changeLevel(int level, bool upordown)
{
	player->currentLevel = level;

	int windowW, windowH;
	Engine::GetInstance().window.get()->GetWindowSize(windowW, windowH);

	int texW, texH;
	std::string fileName = "Assets/Textures/fondo" + std::to_string(level) + ".png";

	bg = Engine::GetInstance().textures.get()->Load(fileName.c_str());
	Engine::GetInstance().textures.get()->GetSize(bg, texW, texH);

 	Engine::GetInstance().map->Load("Assets/Maps/", "Tilemap.tmx", level);

	pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemy");
	pugi::xml_node itemNode = configParameters.child("entities").child("player").child("items");
	if (enemyList.size() > 0)
	{
		Engine::GetInstance().entityManager->DestroyEntity(enemyList[0]);
		enemyList.clear();
	}
	if (itemList.size() > 0)
	{
		Engine::GetInstance().entityManager->DestroyEntity(itemList[0]);
		itemList.clear();
	}
	
	bool canSpawnItem = true;
	if (takenItems.size() > 0)
	{
     	for (int i = 0; i < takenItems.size(); i++)
		{
			if (level == takenItems[i])
			{
				canSpawnItem = false;
			}
		}
	}
	switch (level)
	{
	case 1:
		itemNode.attribute("x") = 240;
		itemNode.attribute("y") = 30;
		itemNode.attribute("type") = 1;
		break;
	case 2:
		itemNode.attribute("x") = 424;
		itemNode.attribute("y") = 155;
		itemNode.attribute("type") = 2;
		break;
	case 3:
		itemNode.attribute("x") = 309;
		itemNode.attribute("y") = 135;
		itemNode.attribute("type") = 3;
		break;
	case 4:
		itemNode.attribute("x") = 436;
		itemNode.attribute("y") = 95;
		itemNode.attribute("type") = 2;
		break;
	case 5:
		itemNode.attribute("x") = 45;
		itemNode.attribute("y") = 215;
		itemNode.attribute("type") = 1;
		break;
	default:
		break;
	}

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	std::string lvl = "lvl" + std::to_string(level);
	if (loadFile.child("config").child("scene").child("entities").child("player").child("items").attribute(lvl.c_str()).as_bool() == false)
	{
		if (canSpawnItem == true)
		{
			Item* item = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
			item->SetParameters(itemNode);
			itemList.push_back(item);
		}
	}
	
	if (upordown && level != 1 && level != 6)
	{
		switch (level)
		{
		case 2:
			enemyNode.attribute("x") = 50;
			enemyNode.attribute("y") = 50;
			enemyNode.attribute("gravity") = true;
			grounded = true;
			break;
		case 3:
			enemyNode.attribute("x") = 50;
			enemyNode.attribute("y") = 50;
			enemyNode.attribute("gravity") = false;
			grounded = false;
			break;
		case 4:
			enemyNode.attribute("x") = 380;
			enemyNode.attribute("y") = 50;
			enemyNode.attribute("gravity") = false;
			grounded = false;
			break;
		case 5:
			enemyNode.attribute("x") = 160;
			enemyNode.attribute("y") = 50;
			enemyNode.attribute("gravity") = true;
			grounded = true;
			break;
		default:
			break;
		}

		Enemy* enemy = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY);
		enemy->SetParameters(enemyNode);
		enemyList.push_back(enemy);

		enemy->ChangeGrounded(grounded);
	}
}

bool Scene::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	LOG("Press Gui Control: %d", control->id);

	return true;
}