#include "ClientProjectile.h"

#include "Sprite.h"

void ClientProjectile::Initialize(Texture2D* tex)
{
	m_Sprite = new Sprite(tex, Color::White(), this, 1);
}
