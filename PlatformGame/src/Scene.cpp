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

	//L08 Create a new item using the entity manager and set the position to (200, 672) to test
	//Item* item = (Item*) Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
	//item->position = Vector2D(200, 672);

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

	//enemies
	if (enemyList.size() > 0)
	{
		Vector2D enemyPos = Vector2D(sceneNode.child("entities").child("enemies").child("enemy").attribute("x").as_int(),
			sceneNode.child("entities").child("enemies").child("enemy").attribute("y").as_int());

		enemyList[0]->SetPosition(enemyPos);
	}

	takenItems.clear();

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

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN and player->currentLevel < 5)
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
		if (player->GetPosition().getY() + 7 > enemyList[0]->GetPosition().getY())
		{
			player->Die();
			player->checkDeath = false;
		}
		else
		{
			Engine::GetInstance().entityManager->DestroyEntity(enemyList[0]);
			enemyList.clear();

			player->JumpFX();
			player->KillFX();

			player->pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -0.6), true);
			player->checkDeath = false;
		}
	}

	if (itemList.size() > 0 && player->takeItem == true)
	{
		Engine::GetInstance().entityManager->DestroyEntity(itemList[0]);
		itemList.clear();
		//SONIDO PILLAR ITEM

		takenItems.push_back(player->currentLevel);
		player->takeItem = false;
	}

	if (checkpoint->Saving == true)
	{
		SaveState();
		checkpoint->Saving = false;
	}

	if (player->Loading == true)
	{
		LoadState();
		player->Loading = false;
	}

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
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F5) == KEY_DOWN && player->isDead == false)
	{
		SaveState();
		//Reset checkpoint
		checkpoint->CheckTaken = false;
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

	switch (player->currentLevel)
	{
	case 4:
		newCheck = Vector2D(51, 207);
		break;
	default:
		break;
	}
	checkpoint->SetPosition(newCheck);

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

	sceneNode.child("entities").child("player").attribute("level").set_value(1);
	sceneNode.child("entities").child("player").child("items").attribute("lvl2").set_value(false);
	sceneNode.child("entities").child("player").child("items").attribute("lvl3").set_value(false);
	sceneNode.child("entities").child("player").child("items").attribute("lvl4").set_value(false);
	sceneNode.child("entities").child("player").child("items").attribute("lvl5").set_value(false);
	loadFile.save_file("config.xml");
	takenItems.clear();

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

	sceneNode.child("entities").child("player").attribute("level").set_value(1);
	sceneNode.child("entities").child("player").child("items").attribute("lvl2").set_value(false);
	sceneNode.child("entities").child("player").child("items").attribute("lvl3").set_value(false);
	sceneNode.child("entities").child("player").child("items").attribute("lvl4").set_value(false);
	sceneNode.child("entities").child("player").child("items").attribute("lvl5").set_value(false);
	loadFile.save_file("config.xml");
	takenItems.clear();

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
		break;
	case 2:
		itemNode.attribute("x") = 424;
		itemNode.attribute("y") = 155;
		break;
	case 3:
		itemNode.attribute("x") = 309;
		itemNode.attribute("y") = 135;
		break;
	case 4:
		itemNode.attribute("x") = 436;
		itemNode.attribute("y") = 95;
		break;
	case 5:
		itemNode.attribute("x") = 45;
		itemNode.attribute("y") = 215;
		break;
	default:
		break;
	}

	std::string lvl = "lvl" + std::to_string(level);
	if (itemNode.attribute(lvl.c_str()).as_bool() == false)
	{
		if (canSpawnItem == true)
		{
			Item* item = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
			item->SetParameters(itemNode);
			itemList.push_back(item);
		}
	}

	if (upordown && level != 1)
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