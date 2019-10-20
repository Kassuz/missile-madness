#include "LobbyPlayer.h"

#include "Networking/User.h"
#include "Engine.h"

LobbyPlayer::LobbyPlayer(User* user) : m_User(user)
{
	m_Transform.SetScale(glm::vec3(100.0f));

	Texture2D* tex = ResourceManager::Instance().LoadTexture2D("Resources/Textures/Character.png");
	m_Sprite = new Sprite(tex, user->GetCharacterColor(), this, 0);
}

LobbyPlayer::~LobbyPlayer()
{
	delete m_Sprite;
}

void LobbyPlayer::Update()
{
	if (!m_IsActive) return;

	m_Sprite->SetColor(m_User->GetCharacterColor());

	glm::vec3 pos = m_Transform.GetPosition();
	Color nameCol = m_User->IsReady() ? Color::Green() : Color::Red();
	TextRenderer::Instance().RenderTextWorldSpace(m_User->GetUsersName(), pos.x - 20.0f, pos.y + 55.0f, 0.6f, 0.0f, nameCol);
}
