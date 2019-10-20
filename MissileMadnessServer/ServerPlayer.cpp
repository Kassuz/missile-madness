#include "ServerPlayer.h"

#include "Engine.h"

#include "NetworkManagerServer.h"
#include "Networking/User.h"

#include "Move.h"
#include "ServerGame.h"

void ServerPlayer::Initialize(User* controllingUser, ServerGame* game)
{
	m_User = controllingUser;
	m_Game = game;
}

void ServerPlayer::Update()
{
	if (m_User == nullptr)
	{
		Debug::LogErrorFormat("Couldn't find user with id: %u. User might have disconnected. Player removed!", m_User);
		m_Game->HandlePlayerDisconnect(this);
		return;
	}
	
	glm::vec3 oldPos = m_Transform.GetPosition();

	for (UInt32 i = 0; i < k_MaxMovesPerUpdate; ++i)
	{
		float lastTimestamp = m_User->GetLastProcessedMoveTimestamp();
		Move* move = m_User->GetFirstMove();

		if (move != nullptr)
		{
			// Dead players don't move, but moves should still be "used"
			if (IsDead()) continue;


			float deltaT = move->GetTimestamp() - lastTimestamp;
			ProcessMove(move, deltaT);

			if (move->GetSpaceState())
			{
				//if (m_ProjectileTimer <= 0.0f)
				{
					//Debug::LogWarningFormat("Player %s shoots", user->GetUsersName().c_str());
					ShootProjectile();					
				}
			}


			delete move;
		}
		else
		{
			break;
		}
	}
	glm::vec3 dir = m_Transform.GetPosition() - oldPos;
	if (glm::length(dir) > 1.0f)
		m_VelocityDir = glm::normalize(dir);
	else
		m_VelocityDir = glm::vec3(0.0f);

	//Debug::LogFormat("M_VelocityDir for player %u is: x: %f y: %f z: %f", m_User, m_VelocityDir.x, m_VelocityDir.y, m_VelocityDir.z);
	//m_ProjectileTimer -= Time::deltaTime;
}

bool ServerPlayer::TakeDamage(float damage)
{
	this->m_Health -= damage;

	if (IsDead())
	{
		SetActive(false);
		return true;
	}
	else
	{
		return false;
	}
}

void ServerPlayer::Reset(glm::vec3 respawnPos)
{
	Debug::LogWarning("Player Respawn");
	m_Transform.SetPosition(respawnPos);
	m_Health = k_MaxHealth;
	SetActive(true);
}

void ServerPlayer::ShootProjectile()
{
	m_ProjectileTimer = k_ProjectileCooldown;

	glm::vec3 missilePos = glm::vec3(m_Transform.GetModel() * k_ShootPos);
	m_Game->GetProjectileManager().SpawnProjectile(this, missilePos, m_Transform.GetUp());
}