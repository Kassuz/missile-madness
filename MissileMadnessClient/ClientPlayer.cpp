#include "ClientPlayer.h"

#include "Networking/User.h"
#include "NetworkManagerClient.h"
#include "Engine.h"

#include "CommandLineArgs.h"

#include "DebugPos.h"

ClientPlayer::ClientPlayer(UInt32 networkID) : Player(networkID)
{
	if (CommandLineArgs::ShouldShowDebugPos())
		m_DebugPos = new DebugPos(ResourceManager::Instance().LoadTexture2D("Resources/Textures/DebugPos.png"));
}

ClientPlayer::~ClientPlayer()
{
	delete m_PlayerSprite;

	if (m_DebugPos != nullptr)
		delete m_DebugPos;
}

void ClientPlayer::Read(InputMemoryBitStream& packet)
{
	packet.Read(m_User);

	glm::vec3 pos;
	glm::quat rot;

	packet.Read(pos);
	packet.Read(rot);

	packet.Read(m_Health);
	packet.Read(m_VelocityDir);

	// Move replay in update, because we don't know the latest move processed by server yet

	if (IsLocal())
	{
		m_Transform.SetPosition(pos);
		m_Transform.SetRotation(rot);
	}
	else
	{
		m_StartPos = m_Transform.GetPosition();
		m_Transform.SetPosition(pos);
		m_Transform.SetRotation(rot);
		m_Transform.Translate(m_VelocityDir * k_MovementSpeed * NetworkManagerClient::Instance().GetRTT() * 0.5f);
	}

	m_HasRecievedData = true;

	//if (!m_HasRecievedData || !m_IsActive)
	//{
	//	m_StartPos = m_InterpolatePos;
	//	m_StartRot = m_InterpolateRot;
	//	m_HasRecievedData = true;
	//}
	//else
	//{
	//	m_StartPos = m_Transform.GetPosition();
	//	m_StartRot = m_Transform.GetRotation();
	//}

	m_LastDataTime = Time::GetTime();

	if (m_DebugPos != nullptr)
	{
		m_DebugPos->GetTransform()->SetPosition(pos);
		m_DebugPos->GetTransform()->SetRotation(rot);
	}
}

void ClientPlayer::Initialize(Texture2D* playerTexture)
{
	m_PlayerSprite = new Sprite(playerTexture, Color::White(), this, 0);
	//m_Transform.SetScale(glm::vec3(k_PlayerScale));
}

bool ClientPlayer::IsLocal()
{
	return User::Me->GetUserID() == m_User;
}

void ClientPlayer::Update(float dataIntervall)
{
	if (!m_IsActive) return;

	if (IsLocal())
	{
		Move* first = User::Me->GetFirstMove(true); // Peek first move. All moves processed by server are removed
		if (first != nullptr)
		{
			// Replay all moves in queue
			if (m_HasRecievedData)
			{
				float lastTimeStamp = User::Me->GetLastProcessedMoveTimestamp();
				while (first != nullptr)
				{
					float deltaT = first->GetTimestamp() - lastTimeStamp;
					ProcessMove(first, deltaT);

					lastTimeStamp = first->GetTimestamp();
					first = first->next;
				}
				m_HasRecievedData = false;
			}
			else // Play latest move
			{
				while (first->next != nullptr)
					first = first->next;

				ProcessMove(first, Time::deltaTime);
			}
		}
	}
	else
	{
		if (m_HasRecievedData) // Simulate for 1/2 rtt
		{
			m_HasRecievedData = false;
			
			//m_ExtrapolatePos += m_VelocityDir * k_MovementSpeed * NetworkManagerClient::Instance().GetRTT();
			if (glm::length(m_StartPos - m_Transform.GetPosition()) > 0.1f)
			{
				if (m_LocationOutOfSyncTime == 0.0f)
					m_LocationOutOfSyncTime = Time::GetTime();

				float timeOutOfSync = Time::GetTime() - m_LocationOutOfSyncTime;
				float rtt = NetworkManagerClient::Instance().GetRTT();

				if (timeOutOfSync < rtt)
				{
					glm::vec3 pos = glm::mix(m_StartPos, m_Transform.GetPosition(), timeOutOfSync / rtt);
					m_Transform.SetPosition(pos);
				}
			}
			else
			{
				m_LocationOutOfSyncTime = 0.0f;
			}
		}
		else
		{
			m_Transform.Translate(m_VelocityDir * k_MovementSpeed * Time::deltaTime);
		//m_ExtrapolatePos += m_VelocityDir * k_MovementSpeed * Time::deltaTime;

		}

		
	}

	//if (m_HasRecievedData)
	//{
	//	//float timeSinceLastUpdate = Time::GetTime() - m_LastDataTime;
	//	//float t = timeSinceLastUpdate / dataIntervall;
	//	//if (t > 1.0f) t = 1.0f;
	//	//
	//	//glm::vec3 pos = glm::mix(m_StartPos, m_InterpolatePos, t);
	//	//glm::quat rot = glm::slerp(m_StartRot, m_InterpolateRot, t);

	//	//m_Transform.SetPosition(pos);
	//	//m_Transform.SetRotation(rot);
	//}

	User* u = NetworkManagerClient::Instance().GetUserWithID(m_User);
	if (u != nullptr)
	{
		// Draw name
		Color nameCol = IsLocal() ? Color::Green() : Color::Red();

		glm::vec3 pos = m_Transform.GetPosition();
		TextRenderer::Instance().RenderTextWorldSpace(u->GetUsersName(), pos.x - 20.0f, pos.y + 55.0f, 0.6f, 0.0f, nameCol);
	}
}

