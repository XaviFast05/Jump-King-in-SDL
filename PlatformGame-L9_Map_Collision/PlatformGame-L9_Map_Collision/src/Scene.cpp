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
	fondo1 = nullptr;
	fondo2 = nullptr;
	fondo3 = nullptr;
	fondo4 = nullptr;
	fondo5 = nullptr;
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
	Engine::GetInstance().map->Load("Assets/Maps/", "Tilemap.tmx");

	fondo1 = Engine::GetInstance().textures.get()->Load("Assets/Textures/fondo1.png");
	fondo2 = Engine::GetInstance().textures.get()->Load("Assets/Textures/fondo2.png");
	fondo3 = Engine::GetInstance().textures.get()->Load("Assets/Textures/fondo3.png");
	fondo4 = Engine::GetInstance().textures.get()->Load("Assets/Textures/fondo4.png");
	fondo5 = Engine::GetInstance().textures.get()->Load("Assets/Textures/fondo5.png");

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

	int windowW, windowH;
	Engine::GetInstance().window.get()->GetWindowSize(windowW, windowH);

	int texW, texH;

	switch (player->currentLevel)
	{
	case 1:
		Engine::GetInstance().textures.get()->GetSize(fondo1, texW, texH);
		Engine::GetInstance().render.get()->DrawTexture(fondo1, 0, 0);
		break;
	case 2:
		Engine::GetInstance().textures.get()->GetSize(fondo2, texW, texH);
		Engine::GetInstance().render.get()->DrawTexture(fondo2, 0, 0);
		break;
	case 3:
		Engine::GetInstance().textures.get()->GetSize(fondo3, texW, texH);
		Engine::GetInstance().render.get()->DrawTexture(fondo3, 0, 0);
		break;
	case 4:
		Engine::GetInstance().textures.get()->GetSize(fondo4, texW, texH);
		Engine::GetInstance().render.get()->DrawTexture(fondo4, 0, 0);
		break;
	case 5:
		Engine::GetInstance().textures.get()->GetSize(fondo5, texW, texH);
		Engine::GetInstance().render.get()->DrawTexture(fondo5, 0, 0);
		break;
	default:
		break;
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

	SDL_DestroyTexture(fondo1);
	SDL_DestroyTexture(fondo2);
	SDL_DestroyTexture(fondo3);
	SDL_DestroyTexture(fondo4);
	SDL_DestroyTexture(fondo5);

	return true;
}
