#include "bullet.hpp"

Bullet::Bullet(Vector2 origin, Vector2 forwardDir, float speed, bool isPlayerBullet)
    : GameObject({origin.x - BULLET_SIZE / 2, origin.y - BULLET_SIZE / 2, BULLET_SIZE, BULLET_SIZE},
                 0, forwardDir, {}, isPlayerBullet ? BULLET : ENEMY_BULLET)
{
    this->velocity = Vector2Scale(forwardDir, speed);
    this->rotation = atan2(forwardDir.y, forwardDir.x) * RAD2DEG + 90;
    this->texture = ResourceManager::GetSpriteTexture(isPlayerBullet ? BULLET_SPRITE : ENEMY_BULLET_SPRITE);
    this->hitbox = {origin};
    this->isPlayerBullet = isPlayerBullet;
    this->isAlive = true;
}

Bullet::~Bullet()
{
}

void Bullet::Update()
{
    if (!isAlive)
    {
        return;
    }
    if (IsOutOfBounds())
    {
        Destroy();
        return;
    }
    GameObject::Update();
}

void Bullet::Draw()
{
    if (!isAlive)
    {
        return;
    }
    Rectangle dst = {origin.x, origin.y, bounds.width, bounds.height};
    DrawTexturePro(*texture, {0, 0, (float)texture->width, (float)texture->height},
                   dst, {bounds.width / 2, 0}, rotation, WHITE);
}

void Bullet::DrawDebug()
{
    GameObject::DrawDebug();
    DrawCircleLines(origin.x, origin.y, 2, GREEN);
}

void Bullet::HandleCollision(GameObject *other, Vector2 *pushVector)
{
    if (!isAlive)
    {
        return;
    }
    // destroy if bullet hits asteroid or enemy
    if (other->GetType() == ASTEROID || other->GetType() == BASIC_ENEMY)
    {
        Destroy();
    }
    (void)pushVector; // unused parameter
}

bool Bullet::IsOutOfBounds()
{
    const Rectangle worldBox = {-(float)GetScreenWidth() / 2, -(float)GetScreenHeight() / 2, (float)GetScreenWidth(), (float)GetScreenHeight()};
    return !CheckCollisionPointRec(origin, worldBox);   
}
