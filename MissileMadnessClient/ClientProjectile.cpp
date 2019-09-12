#include "ClientProjectile.h"

#include "Engine.h"
#include "ClientGame.h"
#include "NetworkManagerClient.h"
#include "CommandLineArgs.h"
#include "DebugPos.h"

void ClientProjectile::Read(InputMemoryBitStream& packet)
{
	glm::vec3 pos;

	packet.Read(pos);
	packet.Read(m_Velocity);

	if (!m_HasRecievedData)
	{
		m_HasRecievedData = true;

		SetRotation(m_Velocity);
	}

	m_Transform.SetPosition(pos);
	Move(NetworkManagerClient::Instance().GetRTT() / 2.0f); // Simulate movement for 1/2 rtt

	if (m_DebugPos != nullptr)
	{
		m_DebugPos->GetTransform()->SetPosition(pos);
		m_DebugPos->GetTransform()->SetRotation(m_Transform.GetRotation());
	}
}

void ClientProjectile::Initialize(Texture2D* tex)
{
	m_Sprite = new Sprite(tex, Color::White(), this, 1);
}

ClientProjectile::ClientProjectile(UInt32 networkID) : Projectile(networkID) 
{
	if (CommandLineArgs::ShouldShowDebugPos())
		m_DebugPos = new DebugPos(ResourceManager::Instance().LoadTexture2D("Resources/Textures/DebugPos.png"));
}

ClientProjectile::~ClientProjectile()
{
	ClientGame::RemoveProjectile(this);
	delete m_Sprite;

	if (m_DebugPos != nullptr)
		delete m_DebugPos;
}

void ClientProjectile::Update()
{
	Move(Time::deltaTime);
}

