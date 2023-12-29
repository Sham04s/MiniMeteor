#include "asteroid.hpp"
#include "bullet.hpp"
#include "player.hpp"
#include "utils.hpp"

#include "raymath.h"
#include <math.h>

Asteroid::Asteroid(Vector2 origin, AsteroidVariant variant, float velocityMultiplier) : GameObject({0}, 0, {0, -1}, {}, ASTEROID)
{
    this->velocity = {(float)GetRandomValue(-100, 100), (float)GetRandomValue(-100, 100)};
    this->velocity = Vector2Scale(this->velocity, velocityMultiplier);
    this->rotation = GetRandomValue(0, 360);
    this->angularVelocity = GetRandomValue(-10, 10) * 12;
    this->variant = variant;
    this->state = FLOATING;

    this->origin = origin;

    // (2x + 1) -> [1, 3, 5, 7] -> large variants
    // (2x + 2) -> [2, 4, 6, 8] -> small variants
    this->size = (variant == LARGE) ? ASTEROID_SIZE_LARGE : ASTEROID_SIZE_SMALL;
    SpriteTextureID randomAsteroidTexture = (SpriteTextureID)(2 * GetRandomValue(0, 3) + 1); // load large variants
    if (variant == SMALL)
    {
        randomAsteroidTexture = (SpriteTextureID)(randomAsteroidTexture + 1); // load small variants
    }

    this->texture = ResourceManager::GetSpriteTexture(randomAsteroidTexture);
    this->explosionSound = ResourceManager::CreateSoundAlias(EXPLOSION_SOUND);
    SetSoundVolume(explosionSound, size / ASTEROID_SIZE_LARGE); // set volume according to size
    this->bounds = {origin.x - size / 2, origin.y - size / 2, size, size};

    // TODO: maybe use a predefined hitbox for each variant?

    // create hitbox as a regular polygon with 8 sides
    const int sides = 8;
    const float radius = variant == LARGE ? size / 3 : size / 4;
    const float angleIncrement = 2 * PI / sides;

    for (int i = 0; i < sides; i++)
    {
        float angle = i * angleIncrement;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        this->hitbox.push_back(Vector2Add(origin, {x, y}));
    }
}

Asteroid::Asteroid(AsteroidVariant variant, float velocityMultiplier)
: Asteroid(RandomVecOutsideScreen(variant == LARGE ? ASTEROID_SIZE_LARGE : ASTEROID_SIZE_SMALL), variant, velocityMultiplier)
{
}

Asteroid::~Asteroid()
{
    if (explosionSound.frameCount > 0 && explosionSound.stream.buffer != NULL)
    {
        if (IsSoundPlaying(explosionSound))
        {
            StopSound(explosionSound);
        }
        UnloadSoundAlias(explosionSound);
    }
}

void Asteroid::Update()
{
    // change state to DESTROYED after destroying animation is finished
    if (state == EXPLODING && GetTime() - lastExplosionTime > ASTEROID_DESTROY_TIME)
    {
        state = DESTROYED;
    }
    if (state == DESTROYED)
    {
        return;
    }

    // Translation and rotation
    GameObject::Update();

    // Teleport to the other side of the screen if the asteroid goes off-screen
    if (origin.x > GetScreenWidth() + size / 2)
    {
        Translate({-GetScreenWidth() - size, 0});
    }
    else if (origin.x < -size / 2)
    {
        Translate({GetScreenWidth() + size, 0});
    }
    if (origin.y > GetScreenHeight() + size / 2)
    {
        Translate({0, -GetScreenHeight() - size});
    }
    else if (origin.y < -size / 2)
    {
        Translate({0, GetScreenHeight() + size});
    }

    // Play explosion sound if asteroid is exploding
    if (state == EXPLODING && !IsSoundPlaying(explosionSound))
    {
        PlaySound(explosionSound);
    }
}

void Asteroid::Draw()
{
    if (state == DESTROYED)
    {
        return;
    }

    // draw explosion animation if asteroid is exploding
    // the asteroid grows in size and fades out
    if (state == EXPLODING)
    {
        float explosionProgress = (GetTime() - lastExplosionTime) / ASTEROID_EXPLOSION_TIME;
        float scale = 1 + explosionProgress;
        float explosionFade = 1 - explosionProgress;

        this->bounds = {origin.x - scale * size / 2, origin.y - scale * size / 2, scale * size, scale * size};

        DrawTexturePro(*texture, {0, 0, (float)texture->width, (float)texture->height},
                       {origin.x, origin.y, bounds.width, bounds.height}, {size * scale / 2, size * scale / 2}, rotation, Fade(WHITE, explosionFade));

        return;
    }

    GameObject::Draw();
}

void Asteroid::DrawDebug()
{
    GameObject::DrawDebug();
}

void Asteroid::Destroy()
{
    if (state == FLOATING)
    {
        this->state = EXPLODING;
        this->hitbox = {}; // remove hitbox to prevent collisions
        this->lastExplosionTime = GetTime();
    }
}

void Asteroid::HandleCollision(GameObject *other, Vector2 *pushVector)
{
    // destroy if hit by player bullet
    if (other->GetType() == BULLET)
    {
        if (((Bullet *)other)->IsPlayerBullet())
        {
            Destroy();
        }
        return;
    }
    if (other->GetType() == POWER_UP || other->GetType() == PLAYER) // player already handles collision with asteroids
    {
        // do nothing
        return;
    }
    Push(other, *pushVector);
}