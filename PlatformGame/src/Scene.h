#pragma once

#include "Module.h"
#include "Player.h"
#include "Enemy.h"
#include <vector>

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void changeLevel(int level, int previousLevel);

	// Return the player position
	Vector2D GetPlayerPosition();

public:
	// Get tilePosDebug value
	std::string GetTilePosDebug() {
		return tilePosDebug;

	}

private:
	SDL_Texture* bg;
	bool CTVisible = false;
	std::string tilePosDebug = "[0,0]";
	SDL_Texture* CTtexture = nullptr;

	//L03: TODO 3b: Declare a Player attribute
	Player* player;
	std::vector<Enemy*> enemyList;
};