#pragma once

#include "Module.h"
#include "Player.h"
#include "Enemy.h"
#include "CheckPointBF.h"
#include <vector>

struct SDL_Texture;
class Timer;
class CheckPointBF;

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
	//Spawn in level 2
	void SpawnPointLvl2();

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
	int menu_introMS;

	// Declare a Player attribute
	Player* player;
	Enemy* enemy;
	CheckPointBF* checkpoint;
	Timer* timer;
	std::vector<Enemy*> enemyList;

	bool grounded = false;
};