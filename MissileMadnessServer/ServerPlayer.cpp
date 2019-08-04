#include "ServerPlayer.h"

#include "Engine.h"

#include "NetworkManagerServer.h"
#include "Networking/User.h"

#include "Move.h"
#include "ServerGame.h"

void ServerPlayer::Initialize(UInt32 controllingUser, ServerGame* game)
{
	m_User = controllingUser;
	m_Game = game;
}

void ServerPlayer::Update()
{
	User* user = NetworkManagerServer::Instance().GetUserWithID(m_User);
	if (user == nullptr)
	{
		Debug::LogErrorFormat("Couldn't find user with id: %u. User might have disconnected. Player removed!", m_User);
		m_Game->HandlePlayerDisconnect(this);
		return;
	}

	for (UInt32 i = 0; i < k_MaxMovesPerUpdate; ++i)
	{
		Move* move = user->GetFirstMove();

		if (move != nullptr)
		{
			// Dead players don't move, but moves should still be "used"
			if (IsDead()) continue;

			float deltaT = move->GetDeltaT();
			if (move->GetRightState())
			{
				glm::quat rot = glm::angleAxis(-k_RotateSpeed * deltaT, glm::vec3(0.0f, 0.0f, 1.0f));
				m_Transform.Rotate(rot);
			}

			if (move->GetLeftState())
			{
				glm::quat rot = glm::angleAxis(k_RotateSpeed * deltaT, glm::vec3(0.0f, 0.0f, 1.0f));
				m_Transform.Rotate(rot);
			}

			if (move->GetUpState())
			{
				m_Transform.Translate(m_Transform.GetUp() * deltaT * k_MovementSpeed);
			}

			if(move->GetDownState())
			{
				m_Transform.Translate(-m_Transform.GetUp() * deltaT * k_MovementSpeed);
			}

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

	//m_ProjectileTimer -= Time::deltaTime;
}

void ServerPlayer::TakeDamage(float damage)
{

	this->m_Health -= damage;

	if (IsDead())
		SetActive(false);
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