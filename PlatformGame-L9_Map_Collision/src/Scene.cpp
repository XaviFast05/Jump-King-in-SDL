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
	
	//L08 Create a new item using the entity manager and set the position to (200, 672) to test
	Item* item = (Item*) Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
	item->position = Vector2D(200, 672);
	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	//L06 TODO 3: Call the function to load the map. 
	changeLevel(1, -1);

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

	//if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	//	Engine::GetInstance().render.get()->camera.y -= ceil(camSpeed * dt);

	//if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	//	Engine::GetInstance().render.get()->camera.y += ceil(camSpeed * dt);

	//if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	//	Engine::GetInstance().render.get()->camera.x -= ceil(camSpeed * dt);

	//if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	//	Engine::GetInstance().render.get()->camera.x += ceil(camSpeed * dt);

	Engine::GetInstance().render.get()->DrawTexture(bg, 0, 0);

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN and player->currentLevel < 5)
	{
		changeLevel(player->currentLevel + 1, player->currentColision - 1);
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN and player->currentLevel > 1)
	{
		changeLevel(player->currentLevel - 1, player->currentColision + 1);
	}

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	SDL_DestroyTexture(bg);

	return true;
}

void Scene::changeLevel(int level, int colision)
{
	player->currentLevel = level;
	player->currentColision = colision;
	int windowW, windowH;
	Engine::GetInstance().window.get()->GetWindowSize(windowW, windowH);

	int texW, texH;
	std::string fileName = "Assets/Textures/fondo" + std::to_string(level) + ".png";

	bg = Engine::GetInstance().textures.get()->Load(fileName.c_str());
	Engine::GetInstance().textures.get()->GetSize(bg, texW, texH);

	Engine::GetInstance().map->Load("Assets/Maps/", "Tilemap.tmx", level);
	Engine::GetInstance().map->Load("Assets/Maps/", "Tilemap.tmx", colision);
}