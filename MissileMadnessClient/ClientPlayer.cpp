#include "ClientPlayer.h"

#include "Networking/User.h"

void ClientPlayer::Initialize(Texture2D* playerTexture)
{
	m_PlayerSprite = new Sprite(playerTexture, Color::White(), this, 0);
	m_Transform.SetScale(glm::vec3(k_PlayerScale));
}

bool ClientPlayer::IsLocal()
{
	return User::Me->GetUserID() == m_User;
}
