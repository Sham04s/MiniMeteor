#include "bullet.hpp"

Bullet::Bullet(Vector2 origin, Vector2 forwardDir) : GameObject({origin.x - BULLET_SIZE / 2, origin.y - BULLET_SIZE / 2, BULLET_SIZE, BULLET_SIZE}, 0, forwardDir, 0, {}, BULLET)
{
    this->velocity = Vector2Scale(forwardDir, BULLET_SPEED);
    this->rotation = atan2(forwardDir.y, forwardDir.x) * RAD2DEG + 90;
    this->texture = ResourceManager::GetSpriteTexture(BULLET_SPRITE);

    // hitbox is only origin point
}

Bullet::~Bullet()
{
}

void Bullet::Update()
{
    Translate(Vector2Scale(velocity, GetFrameTime()));
}

void Bullet::Draw()
{
    GameObject::Draw();
}

void Bullet::DrawDebug()
{
    GameObject::DrawDebug();
}

bool Bullet::isOutOfBounds()
{
    return (bounds.x < -BULLET_SIZE || bounds.x > GetScreenWidth() + BULLET_SIZE || bounds.y <- BULLET_SIZE || bounds.y > GetScreenHeight() + BULLET_SIZE);
}

bool Bullet::isColliding(GameObject *other)
{
    return CheckCollisionPointPoly(origin, other->GetHitbox().data(), other->GetHitbox().size());
}
