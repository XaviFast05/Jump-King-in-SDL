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

	void changeLevel(int level, bool upordown);

	// Return the player position
	Vector2D GetPlayerPosition();

	//L15 TODO 1: Implement the Load function
	void LoadState();
	//L15 TODO 2: Implement the Save function
	void SaveState();
	//Reset to original position
	void SpawnPoint();

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
	bool once = false;

	//L03: TODO 3b: Declare a Player attribute
	Player* player;
	Enemy* enemy;
	std::vector<Enemy*> enemyList;
};