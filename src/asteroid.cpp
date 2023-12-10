#include "asteroid.hpp"

#include "raymath.h"
#include <math.h>

Asteroid::Asteroid(Vector2 origin) : GameObject({origin.x - 32, origin.y - 32, 64, 64}, 0, {0, -1}, {}, ASTEROID)
{
    this->velocity = {(float)GetRandomValue(-100, 100), (float)GetRandomValue(-100, 100)};
    this->rotation = GetRandomValue(0, 360);
    this->angularVelocity = GetRandomValue(-10, 10) * 12;
    this->variant = (AsteroidVariant)GetRandomValue(0, 2);
    this->state = FLOATING;

    // (2x + 1) -> [1, 3, 5, 7] -> large variants
    // (2x + 2) -> [2, 4, 6, 8] -> small variants
    // TODO: cosider adding sprites for medium asteroids
    float size = (variant == LARGE) ? ASTEROID_SIZE_LARGE : ASTEROID_SIZE_MEDIUM;
    this->mass = size * size;
    SpriteTextureID randomAsteroidTexture = (SpriteTextureID)(2 * GetRandomValue(0, 3) + 1); // load large variants
    if (variant == SMALL)
    {
        randomAsteroidTexture = (SpriteTextureID)(randomAsteroidTexture + 1); // load small variants
        size = ASTEROID_SIZE_SMALL;
    }

    this->texture = ResourceManager::GetSpriteTexture(randomAsteroidTexture);
    this->bounds = {origin.x - size / 2, origin.y - size / 2, size, size};

    // TODO: maybe use a predefined hitbox for each variant?
    // create hitbox as a regular polygon with 8 sides
    const int sides = 8;
    // const float radius = (randomAsteroidTexture % 2 == 1) ? ((variant == LARGE) ? 32 : 24) : 18;
    const float radius = size / 3;
    const float angleIncrement = 2 * PI / sides;

    for (int i = 0; i < sides; i++)
    {
        float angle = i * angleIncrement;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        this->hitbox.push_back(Vector2Add(origin, {x, y}));
    }
}

Asteroid::~Asteroid()
{
}

void Asteroid::Update()
{
    if (state == EXPLODING && GetTime() - lastExplosionTime > ASTEROID_EXPLOSION_TIME)
    {
        state = DESTROYED;
    }
    if (state == DESTROYED)
    {
        return;
    }

    if (origin.x > GetScreenWidth() + 32)
    {
        origin.x = -32;
    }
    if (origin.x < -32)
    {
        origin.x = GetScreenWidth();
    }
    if (origin.y > GetScreenHeight() + 32)
    {
        origin.y = -32;
    }
    if (origin.y < -32)
    {
        origin.y = GetScreenHeight();
    }
    Rotate(angularVelocity * GetFrameTime());
    Translate(Vector2Scale(velocity, GetFrameTime()));

    if (GetOrigin().x > GetScreenWidth() + 32)
    {
        Translate({(float)-GetScreenWidth() - 64, 0});
    }
    if (GetOrigin().x < -32)
    {
        Translate({(float)GetScreenWidth() + 64, 0});
    }
    if (GetOrigin().y > GetScreenHeight() + 32)
    {
        Translate({0, (float)-GetScreenHeight() - 64});
    }
    if (GetOrigin().y < -32)
    {
        Translate({0, (float)GetScreenHeight() + 64});
    }

    // apply collision response
    // if (this->enteredCollision && this->lastCollisionObject->GetType() == ASTEROID)
    // {
    //     Asteroid *asteroid = (Asteroid *)this->lastCollisionObject;

    //     float u = Vector2Length(Vector2Subtract(this->velocity, asteroid->GetVelocity()));                            // relative velocity
    //     float e = 0.7f;                                                                                               // coefficient of restitution
    //     float mA = 50;                                                                                                // mass of this asteroid
    //     float mP = 50;                                                                                                // mass of other asteroid
    //     float angle = Vector2Angle(Vector2Subtract(asteroid->GetOrigin(), this->lastCollisionPoint), this->velocity); // angle between this asteroid and other asteroid
    //     float q = (mA / mP);                                                                                          // mass ratio

    //     float f = (1 + e) / (2 * q + 1 + pow(sin(angle), 2));

    //     float va = f * u;               // velocity of asteroid after collision
    //     float vp = (1 - 2 * q * f) * u; // velocity of other asteroid after collision

    //     float w = (f * sin(angle) * u) / Vector2Length(Vector2Subtract(asteroid->GetOrigin(), this->lastCollisionPoint)); // angular velocity of asteroid after collision

    //     asteroid->SetVelocity(Vector2Scale(Vector2Normalize(Vector2Subtract(asteroid->GetOrigin(), this->lastCollisionPoint)), va));
    //     asteroid->SetAngularVelocity(w * RAD2DEG);

    //     this->velocity = Vector2Scale(Vector2Normalize(Vector2Subtract(this->origin, this->lastCollisionPoint)), vp);
    //     this->ResetCollisionChecks();
    // }
}

void Asteroid::Draw()
{
    if (state == EXPLODING)
    {
        float explosionProgress = (GetTime() - lastExplosionTime) / ASTEROID_EXPLOSION_TIME;
        float scale = 1 + explosionProgress;
        float explosionFade = 1 - explosionProgress;
        float size = (variant == LARGE)
                         ? ASTEROID_SIZE_LARGE
                         : ((variant == MEDIUM)
                                ? ASTEROID_SIZE_MEDIUM
                                : ASTEROID_SIZE_SMALL);

        this->bounds = {origin.x - scale * size / 2, origin.y - scale * size / 2, scale * size, scale * size};

        DrawTexturePro(texture, {0, 0, (float)texture.width, (float)texture.height},
                       {origin.x, origin.y, bounds.width, bounds.height}, {size * scale / 2, size * scale / 2}, rotation, Fade(WHITE, explosionFade));

        return;
    }
    if (state == DESTROYED)
    {
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
        this->hitbox = {};
        this->lastExplosionTime = GetTime(); // TODO: consider implementing custom timer to be able to pause the game
    }
}

void Asteroid::SetVelocity(Vector2 velocity)
{
    this->velocity = velocity;
}

void Asteroid::SetRotation(float rotation)
{
    this->rotation = rotation;
}

void Asteroid::SetAngularVelocity(float angularVelocity)
{
    this->angularVelocity = angularVelocity;
}

Vector2 Asteroid::GetVelocity()
{
    return velocity;
}

float Asteroid::GetRotation()
{
    return rotation;
}

float Asteroid::GetAngularVelocity()
{
    return angularVelocity;
}
