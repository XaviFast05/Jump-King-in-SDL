#include "Enemy.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "Map.h"
#include "EntityManager.h"

Enemy::Enemy() : Entity(EntityType::ENEMY)
{
	map = Engine::GetInstance().map.get();
}

Enemy::~Enemy() {
	delete pathfinding;
}

bool Enemy::Awake() {
	return true;
}

bool Enemy::Start() 
{
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());

	if (!hasStarted)
	{
		//Add a physics to an item - initialize the physics body
		pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, 8, bodyType::DYNAMIC);
	}
	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());

	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	flying.LoadAnimations(parameters.child("animations").child("flying"));
	idleGrounded.LoadAnimations(parameters.child("animations").child("idleGrounded"));
	flyingGrounded.LoadAnimations(parameters.child("animations").child("flyingGrounded"));
	if (!isGrounded)
	{
		currentAnimation = &idle;
	}
	else if (isGrounded)
	{
		currentAnimation = &idleGrounded;
	}

	//Assign collider type
	pbody->ctype = ColliderType::ENEMY;

	pbody->listener = this;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	chaseFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Enemy/bird_chase.wav");

	Mix_Volume(chaseFxId, 200);

	// Initialize pathfinding
	pathfinding = new Pathfinding();
	ResetPath();
	hasStarted = true;
	return true;
}

bool Enemy::Update(float dt)
{
	int maxIterations = 13; // Max number of iterations to avoid crashes
	int iterations = 0;
	
	if (!isGrounded)
	{
		currentAnimation = &idle;
	}
	else if (isGrounded)
	{
		currentAnimation = &idleGrounded;
	}
	//printf("%i ,%i\n", pbody->body->GetPosition().x, pbody->body->GetPosition().y);

 	pathfinding->layerNav = map->GetNavigationLayer();

	//Reset and propagate the pathfanding to follow the player
	Vector2D enemyPos = GetPosition();
	Vector2D playerPos = Engine::GetInstance().scene.get()->GetPlayerPosition();
	Vector2D playerTilePos = Engine::GetInstance().map.get()->WorldToMap(playerPos.getX(), playerPos.getY());

	pathfinding->ResetPath(Engine::GetInstance().map.get()->WorldToMap(enemyPos.getX(), enemyPos.getY()));

	while (pathfinding->pathTiles.empty() && iterations < maxIterations) {
		pathfinding->PropagateAStar(SQUARED);
		iterations++;
	}


	//Get last tile
	if (pathfinding->pathTiles.size() > 3) {
		Vector2D targetTile = pathfinding->pathTiles[pathfinding->pathTiles.size() - 4];
		Vector2D targetWorldPos = Engine::GetInstance().map.get()->MapToWorld(targetTile.getX(), targetTile.getY());

		//Calculate vector movement
		Vector2D movement = targetWorldPos + Vector2D(16, 16) - enemyPos;
		float distance = movement.magnitude();

		if (distance > 1.0f) { //If distance is bigger than
			movement.normalized();
			if (!isGrounded)
			{
				b2Vec2 velocity(movement.getX() * 0.05f, movement.getY() * 0.05f); // Aplicar velocidad en X e Y
				pbody->body->SetLinearVelocity(velocity);
			}
			else if (isGrounded)
			{
				b2Vec2 velocity(movement.getX() * 0.05f, 2); // Aplicar velocidad en X
				pbody->body->SetLinearVelocity(velocity);
			}
			IsSearching = true;
		}
	}

	if (enemyPos.getX() > playerPos.getX())
	{
		flipSprite = true;
		if (flipSprite == true && hflip == SDL_FLIP_NONE) {
			hflip = SDL_FLIP_HORIZONTAL;
		}
	}
	else
	{
		flipSprite = false;
		if (flipSprite == false && hflip == SDL_FLIP_HORIZONTAL) {
			hflip = SDL_FLIP_NONE;
		}
	}

	if (IsSearching == true)
	{
		if (!isGrounded)
		{
			currentAnimation = &flying;
			x++;
		}
		else if (isGrounded)
		{
			currentAnimation = &flyingGrounded;
			y++;
		}
		
	}

	if (currentAnimation == &flying && x == 1 )
	{
		Engine::GetInstance().audio->PlayFx(chaseFxId);
		x++;
	}

	if (currentAnimation == &flyingGrounded &&  y == 1)
	{
		Engine::GetInstance().audio->PlayFx(chaseFxId);
		x++;
	}



	// L08 TODO 4: Add a physics to an item - update the position of the object from the physics.  
	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame(), hflip);
	currentAnimation->Update();

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
	{
		DrawingPath = !DrawingPath;
	}

	// Draw pathfinding 
	if (DrawingPath) pathfinding->DrawPath();

	return true;
}

bool Enemy::CleanUp()
{
	Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
	return true;
}

bool Enemy::DeleteBody()
{
	Engine::GetInstance().physics.get()->DeleteBody(pbody);
	return true;
}

void Enemy::SetPosition(Vector2D pos) {
	pos.setX(pos.getX());
	pos.setY(pos.getY());
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Enemy::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void Enemy::ResetPath() {
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	pathfinding->ResetPath(tilePos);
}

void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		break;
	case ColliderType::PLAYERSENSOR:
		//isDead = true;
		break;
	}
}

void Enemy::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		break;
	}
}

float Suavizar(float start, float end, float alpha) {
    return start + alpha * (end - start);
}

void Enemy::ChangeGrounded(bool grounded)
{
	isGrounded = grounded;
}