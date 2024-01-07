#include "game/objects/asteroid.hpp"
#include "game/objects/bullet.hpp"
#include "game/objects/player.hpp"
#include "utils/utils.hpp"

#include "raymath.h"
#include <math.h>

#include "map"

// ------------------ Asteroids hitboxes ------------------
const Vector2 large[] = {
    {0.000f, -0.378f},
    {0.250f, -0.252f},
    {0.375f, 0.000f},
    {0.250f, 0.252f},
    {0.000f, 0.375f},
    {-0.250f, 0.250f},
    {-0.378f, 0.002f},
    {-0.253f, -0.250f},
};

const Vector2 small[] = {
    {0.002f, -0.245f},
    {0.185f, -0.188f},
    {0.250f, -0.003f},
    {0.187f, 0.180f},
    {-0.002f, 0.245f},
    {-0.183f, 0.187f},
    {-0.253f, -0.003f},
    {-0.188f, -0.188f},
};

const Vector2 sqr_large[] = {
    {0.022f, -0.380f},
    {0.378f, -0.127f},
    {0.375f, 0.003f},
    {0.125f, 0.372f},
    {-0.002f, 0.373f},
    {-0.378f, 0.123f},
    {-0.382f, 0.000f},
    {-0.123f, -0.375f},
};

const Vector2 sqr_small[] = {
    {-0.002f, -0.253f},
    {0.252f, -0.123f},
    {0.250f, 0.002f},
    {0.128f, 0.250f},
    {-0.003f, 0.253f},
    {-0.250f, 0.123f},
    {-0.253f, 0.002f},
    {-0.128f, -0.252f},
};

const Vector2 *asteroid_shapes[] = {
    large,
    small,
    sqr_large,
    sqr_small,
};

// --------------------------------------------------------

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
        randomAsteroidTexture = (SpriteTextureID)(randomAsteroidTexture + ASTEROID_DETAILED_LARGE_SPRITE); // load small variants
    }

    this->texture = ResourceManager::GetSpriteTexture(randomAsteroidTexture);
    this->explosionSound = ResourceManager::CreateSoundAlias(EXPLOSION_SOUND);
    SetSoundVolume(explosionSound, size / ASTEROID_SIZE_LARGE); // set volume according to size
    this->bounds = {origin.x - size / 2, origin.y - size / 2, size, size};

    int shape = randomAsteroidTexture - ASTEROID_DETAILED_LARGE_SPRITE;

    //    normal/squared  large/small
    shape = 2 * (shape / 4) + shape % 2;

    for (int i = 0; i < 8; i++)
    {
        this->hitbox.push_back(Vector2Add(origin, Vector2Scale(Vector2Rotate(asteroid_shapes[shape][i], rotation * DEG2RAD), size)));
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
    const Rectangle worldBox = {-(float)GetScreenWidth() / 2, -(float)GetScreenHeight() / 2, (float)GetScreenWidth(), (float)GetScreenHeight()};
    if (origin.x > worldBox.x + worldBox.width + size / 2)
    {
        Translate({-worldBox.width - size, 0});
    }
    else if (origin.x < worldBox.x - size / 2)
    {
        Translate({worldBox.width + size, 0});
    }
    if (origin.y > worldBox.y + worldBox.height + size / 2)
    {
        Translate({0, -worldBox.height - size});
    }
    else if (origin.y < worldBox.y - size / 2)
    {
        Translate({0, worldBox.height + size});
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
        this->hitbox.clear(); // remove hitbox to prevent collisions
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