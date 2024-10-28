#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Box2D/Box2D.h"
#include "Animation.h"

struct SDL_Texture;

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	// L08 TODO 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

public:

	//Declare player parameters
	float speed = 5.0f;
	SDL_Texture* texture = NULL;
	int texW, texH;

	//Audio fx
	int jumpFxId;
	int landFxId;
	int splatFxId;
	// For landFxId
	int x = 0;

	// L08 TODO 5: Add physics to the player - declare a Physics body
	PhysBody* pbody;
	PhysBody* pbodyBody;
	float jumpForce = 1.1f; // The force to apply when jumping
	bool isJumping = false; // Flag to check if the player is currently jumping
	bool isFalling = false;
	bool flipSprite = false;
	bool running = false;
	bool isSplatted = false;
	bool debug_;

	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idle;
	Animation move;
	Animation jumping;
	Animation falling;
	Animation splatted;

	SDL_RendererFlip hflip = SDL_FLIP_NONE;

	int currentLevel = 1;
	int maxLevel = 5;

	void ascend(bool upDown);
	int levelsFallen = 0;
};