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
	
	//L08 Create a new item using the entity manager and set the position to (200, 672) to test
	Item* item = (Item*) Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
	item->position = Vector2D(200, 672);
	player->position = Vector2D(0, 0);

	// Create a enemy using the entity manager 
	for (pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
	{
		Enemy* enemy = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY);
		enemy->SetParameters(enemyNode);
		enemyList.push_back(enemy);
	}

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	//L06 TODO 3: Call the function to load the map. 
	Engine::GetInstance().map->Load("Assets/Maps/", "Tilemap.tmx", 1);

	CTtexture = Engine::GetInstance().textures->Load("Assets/Textures/CONTROLS.png");
	changeLevel(1, 1);

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
		changeLevel(player->currentLevel + 1, player->currentLevel);
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN and player->currentLevel > 1)
	{
		changeLevel(player->currentLevel - 1, player->currentLevel);
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_H) == KEY_DOWN)
	{
		CTVisible = !CTVisible;
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
		LoadState();

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		SaveState();

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
	SDL_DestroyTexture(bg);
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

	//enemies
	Vector2D enemyPos = Vector2D(sceneNode.child("entities").child("enemy").attribute("x").as_int(),
		sceneNode.child("entities").child("enemy").attribute("y").as_int());
	//enemy->SetPosition(enemyPos);

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

	//Save info to XML 

	//Player position
	sceneNode.child("entities").child("player").attribute("x").set_value(player->GetPosition().getX());
	sceneNode.child("entities").child("player").attribute("y").set_value(player->GetPosition().getY());

	//enemies
	//sceneNode.child("entities").child("enemy").attribute("x").set_value(enemy->GetPosition().getX());
	//sceneNode.child("entities").child("enemy").attribute("y").set_value(enemy->GetPosition().getY());

	//Saves the modifications to the XML 
	loadFile.save_file("config.xml");
}

void Scene::changeLevel(int level, int previousLevel)
{
	player->currentLevel = level;

	int windowW, windowH;
	Engine::GetInstance().window.get()->GetWindowSize(windowW, windowH);

	int texW, texH;
	std::string fileName = "Assets/Textures/fondo" + std::to_string(level) + ".png";

	bg = Engine::GetInstance().textures.get()->Load(fileName.c_str());
	Engine::GetInstance().textures.get()->GetSize(bg, texW, texH);

	Engine::GetInstance().map->Load("Assets/Maps/", "Tilemap.tmx", level);
}