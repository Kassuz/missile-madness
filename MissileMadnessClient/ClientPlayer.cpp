#include "ClientPlayer.h"

#include "Networking/User.h"
#include "NetworkManagerClient.h"
#include "Engine.h"

void ClientPlayer::Initialize(Texture2D* playerTexture)
{
	m_PlayerSprite = new Sprite(playerTexture, Color::White(), this, 0);
	//m_Transform.SetScale(glm::vec3(k_PlayerScale));
}

bool ClientPlayer::IsLocal()
{
	return User::Me->GetUserID() == m_User;
}

void ClientPlayer::Update()
{
	if (!m_IsActive) return;

	User* u = NetworkManagerClient::Instance().GetUserWithID(m_User);
	if (u != nullptr)
	{
		// Draw name
		Color nameCol = IsLocal() ? Color::Green() : Color::Red();

		glm::vec3 pos = m_Transform.GetPosition();
		TextRenderer::Instance().RenderTextWorldSpace(u->GetUsersName(), pos.x - 20.0f, pos.y + 55.0f, 0.6f, 0.0f, nameCol);
	}
}
