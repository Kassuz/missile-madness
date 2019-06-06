#include "Game.h"

#include "Engine.h"
#include "Player.h"
#include "Projectile.h"
#include "ProjectileManager.h"

#include <glm/gtc/random.hpp>

Game::Game(UInt32 screenWidth, UInt32 screenHeight, UInt32 playerCount)
{
	Texture2D* playerTexture = ResourceManager::Instance().LoadTexture2D("Resources/Textures/Character.png");

	for (UInt32 i = 0; i < playerCount; ++i)
	{
		Player* p = new Player(playerTexture, 100.0f, i == 0);

		float posX = std::cos((i / (float)playerCount) * glm::two_pi<float>());
		float posY = std::sin((i / (float)playerCount) * glm::two_pi<float>());

		p->GetTransform()->SetPosition(glm::vec3(posX, posY, 0.0f) * 250.0f);

		players.push_back(p);
	}

	glm::vec2 bLeft(-(float)screenWidth / 2.0f, -(float)screenHeight / 2.0f);
	glm::vec2 tLeft(-(float)screenWidth / 2.0f, (float)screenHeight / 2.0f);
	glm::vec2 tRight((float)screenWidth / 2.0f, (float)screenHeight / 2.0f);
	glm::vec2 bRight((float)screenWidth / 2.0f, -(float)screenHeight / 2.0f);

	walls[0] = Wall(tLeft, tRight, glm::vec2(0.0f, -1.0f));
	walls[1] = Wall(bRight, tRight, glm::vec2(-1.0f, 0.0f));
	walls[2] = Wall(bLeft, bRight, glm::vec2(0.0f, 1.0f));
	walls[3] = Wall(bLeft, tLeft, glm::vec2(1.0f, 0.0f));


}


Game::~Game()
{
	for (auto it = players.begin(); it != players.end(); ++it)
	{
		delete (*it);
	}
}

void Game::Update()
{
	// Update players

	for (auto it = players.begin(); it != players.end(); ++it)
	{
		(*it)->Update();
	}

	// Update projectiles
	ProjectileManager::Instance().Update();

	// Handle all collisions
	HandleProjectileCollisons();
	HandlePlayerToPlayerCollisions();
	HandleWallCollisions();

	// Check respawns
	RespawnPlayers();

	//TextRenderer::Instance().RenderText("Hello World!", 100, 100, 1);
}

void Game::HandleWallCollisions()
{
	for (auto it = players.begin(); it != players.end(); ++it)
	{
		if (!(*it)->IsActive()) continue;
		for (int i = 0; i < 4; ++i)
		{
			Transform* playerTransform = (*it)->GetTransform();
			glm::vec2 playerPos = glm::vec2(playerTransform->GetPosition());
			glm::vec2 pointOnWall = ExtraMath::ClosestPointOnLineSegment(playerPos, walls[i].GetStart(), walls[i].GetEnd());
			glm::vec2 toPlayer = playerPos - pointOnWall;

			float dist = glm::length(toPlayer);

			//Debug::Log("Player dist from top: " + std::to_string(dist));
			//Debug::Log("Player pos: " + std::to_string(playerPos.x) + " " + std::to_string(playerPos.y));
			//Debug::Log("Wall pos: " + std::to_string(pointOnWall.x) + " " + std::to_string(pointOnWall.y));

			if (glm::dot(toPlayer, walls[i].GetNormal()) < 0.0f)
			{
				glm::vec3 dir(walls[i].GetNormal().x, walls[i].GetNormal().y, 0.0f);
				playerTransform->Translate(dir * (dist + playerColliderRadius) * Time::deltaTime * collisionPushForce);
			}
			else if (dist < playerColliderRadius)
			{
				glm::vec3 dir(walls[i].GetNormal().x, walls[i].GetNormal().y, 0.0f);
				playerTransform->Translate(dir * dist * Time::deltaTime * collisionPushForce);
			}
		}
	}
}

void Game::HandlePlayerToPlayerCollisions()
{
	Transform* p1Trans, * p2Trans;
	for (auto p1 = players.begin(); p1 != players.end(); ++p1)
	{
		if (!(*p1)->IsActive()) continue;

		p1Trans = (*p1)->GetTransform();
		for (auto p2 = players.begin(); p2 != players.end(); ++p2)
		{
			if (!(*p2)->IsActive()) continue;

			if (p1 == p2) continue;
			p2Trans = (*p2)->GetTransform();
			
			glm::vec3 toP2 = p2Trans->GetPosition() - p1Trans->GetPosition();
			float dist = glm::length(toP2);

			if (dist < 2.0f * playerColliderRadius)
			{
				float mult = 10.0f * playerColliderRadius / dist;
				glm::vec3 pushForce = glm::normalize(toP2) * collisionPushForce * mult * Time::deltaTime;
				p2Trans->Translate(pushForce);
				p1Trans->Translate(-pushForce);
			}
		}
	}
}

void Game::HandleProjectileCollisons()
{
	auto projectiles = ProjectileManager::Instance().GetProjectiles();
	Transform* playerTrans, * projectileTrans;

	
	for (auto pr = projectiles.begin(); pr != projectiles.end(); ++pr)
	{
		projectileTrans = (*pr)->GetTransform();
		for (auto pl = players.begin(); pl != players.end(); ++pl)
		{
			if (!(*pl)->IsActive()) continue;

			if ((*pr)->GetOwner() == (*pl)) continue;

			playerTrans = (*pl)->GetTransform();

			glm::vec3 toProjectile = projectileTrans->GetPosition() - playerTrans->GetPosition();
			float dist = glm::length(toProjectile);

			if (dist < missileColliderRadius + playerColliderRadius)
			{
				Debug::LogError("MISSILE HIT");
				(*pr)->ProjectileHit();
				(*pl)->TakeDamage((*pr)->GetProjectileDamage());
				break; // this missile should not hit anything anymore
			}
		}
	}
}

void Game::RespawnPlayers()
{
	// Check if players should respawn
	for (auto it = respawns.begin(); it != respawns.end(); /**/)
	{
		if (it->second < Time::GetTime())
		{
			glm::vec2 pos = glm::circularRand<float>(250.0f);
			it->first->Reset(glm::vec3(pos.x, pos.y, 0.0f));

			it = respawns.erase(it);
		}
		else
		{
			++it;
		}
	}

	// Check if there are players who died this frame
	for (auto it = players.begin(); it != players.end(); ++it)
	{
		Player* p = (*it);
		if (p->IsDead() && respawns.find(p) == respawns.end())
		{
			respawns.emplace(p, Time::GetTime() + respawnTime);
			Debug::Log("Player should respawn at " + std::to_string(Time::GetTime() + respawnTime));
		}
	}
}
