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

	packet.Read(m_InterpolatePos);
	packet.Read(m_InterpolateRot);

	packet.Read(m_Health);
	if (!m_HasRecievedData || !m_IsActive)
	{
		m_StartPos = m_InterpolatePos;
		m_StartRot = m_InterpolateRot;
		m_HasRecievedData = true;
	}
	else
	{
		m_StartPos = m_Transform.GetPosition();
		m_StartRot = m_Transform.GetRotation();
	}

	m_LastDataTime = Time::GetTime();

	if (m_DebugPos != nullptr)
	{
		m_DebugPos->GetTransform()->SetPosition(m_InterpolatePos);
		m_DebugPos->GetTransform()->SetRotation(m_InterpolateRot);
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

	if (m_HasRecievedData)
	{
		float timeSinceLastUpdate = Time::GetTime() - m_LastDataTime;
		float t = timeSinceLastUpdate / dataIntervall;
		if (t > 1.0f) t = 1.0f;
		
		glm::vec3 pos = glm::mix(m_StartPos, m_InterpolatePos, t);
		glm::quat rot = glm::slerp(m_StartRot, m_InterpolateRot, t);

		m_Transform.SetPosition(pos);
		m_Transform.SetRotation(rot);
	}

	User* u = NetworkManagerClient::Instance().GetUserWithID(m_User);
	if (u != nullptr)
	{
		// Draw name
		Color nameCol = IsLocal() ? Color::Green() : Color::Red();

		glm::vec3 pos = m_Transform.GetPosition();
		TextRenderer::Instance().RenderTextWorldSpace(u->GetUsersName(), pos.x - 20.0f, pos.y + 55.0f, 0.6f, 0.0f, nameCol);
	}
}

