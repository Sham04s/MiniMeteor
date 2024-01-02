#include "character.hpp"

Character::Character(Vector2 origin)
    : GameObject({origin.x - CHARACTER_SIZE / 2, origin.y - CHARACTER_SIZE / 2, CHARACTER_SIZE, CHARACTER_SIZE}, 0, {0, -1}, {}, NONE)
{
    this->lives = CHARACTER_MAX_LIVES;
    this->state = IDLE;
    this->bullets = {};
    this->lastShotTime = 0;
    this->lastDeathTime = 0;
    this->maxSpeed = CHARACTER_MAX_SPEED;
    this->acceleration = CHARACTER_ACCELERATION;
    this->deceleration = CHARACTER_DECELERATION;
    this->turnSpeed = CHARACTER_TURN_SPEED;
    this->texture = ResourceManager::GetDefaultTexture();
    this->timeAccelerating = 0;
    this->exploded = false;
    this->pitchAndVolumeScale = 0.7f;

    this->shootCooldown = CHARACTER_SHOOT_COOLDOWN;
    this->bulletsPerShot = 1;
    this->bulletsSpeed = BULLET_SPEED;
    this->bulletsSpread = BULLET_SPREAD;

    this->shootSound = {{0}};
    this->thrustSound = {{0}};
    this->explosionSound = {{0}};

    SetDefaultHitBox();
}

Character::~Character()
{
    if (thrustSound.frameCount > 0 && thrustSound.stream.buffer != NULL)
    {
        if (IsSoundPlaying(thrustSound))
        {
            StopSound(thrustSound);
        }
        UnloadSoundAlias(thrustSound);
    }
    if (shootSound.frameCount > 0 && shootSound.stream.buffer != NULL)
    {
        if (IsSoundPlaying(shootSound))
        {
            StopSound(shootSound);
        }
        UnloadSoundAlias(shootSound);
    }
    if (explosionSound.frameCount > 0 && explosionSound.stream.buffer != NULL)
    {
        if (IsSoundPlaying(explosionSound))
        {
            StopSound(explosionSound);
        }
        UnloadSoundAlias(explosionSound);
    }
}

void Character::Update()
{

    // update bullets no matter what
    for (size_t i = 0; i < bullets.size(); i++)
    {
        bullets[i].Update();
    }

    if (state & ~IDLE)
    {
        state &= ~IDLE;
    } else
    {
        state = IDLE;
    }

    // move character
    GameObject::Update();

    // if dead, do nothing else
    if (state & DEAD)
    {
        return;
    }

    // if dying, wait for dying animation to finish
    if (state & DYING && GetTime() - lastDeathTime > CHARACTER_DYING_TIME)
    {
        state = DEAD;
    }

    if (state & ACCELERATING)
    {
        if (!IsSoundPlaying(thrustSound))
        {
            StopSound(thrustSound);
            PlaySound(thrustSound);
        }
        timeAccelerating += GetFrameTime();

        // decrease pitch proportional to time accelerating
        const float pitch = fmaxf(THRUST_MIN_PITCH, 1.0f - timeAccelerating / THRUST_PITCH_DECAYING_TIME);
        SetSoundPitch(thrustSound, Clamp(pitch, 0, 1) * pitchAndVolumeScale);

        // move sound from right to left proportional to player position in x axis
        const float pan = (THRUST_MAX_PAN - TRHUST_MIN_PAN) * (1.0f - origin.x / GetScreenWidth()) + TRHUST_MIN_PAN;
        SetSoundPan(thrustSound, Clamp(pan, 0, 1));

        // decrease volume proportional to player position in y axis
        const float volume = 1.0f - 2.0f * (1.0f - THRUST_MIN_VOLUME) * fabsf(0.5f - origin.y / GetScreenHeight());
        SetSoundVolume(thrustSound, Clamp(volume, 0, 1) * pitchAndVolumeScale);
    }
    else
    {
        StopSound(thrustSound);
        timeAccelerating = 0;
    }

    if (IsSoundReady(explosionSound))
    {
        if (!IsAlive() && !exploded)
        {
            exploded = true;
            PlaySound(explosionSound);
        }
        if (IsAlive() && exploded)
        {
            exploded = false;
            StopSound(explosionSound);
        }
    }

    if (state & ACCELERATING)
    {
        Accelerate(acceleration);
    }

    // decelerate character if above max speed
    if (Vector2Length(velocity) > maxSpeed)
    {
        // decelerate
        velocity = Vector2Subtract(velocity, Vector2Scale(Vector2Normalize(velocity), deceleration * GetFrameTime()));
    }

    // rotate character
    if (state & TURNING_LEFT)
    {
        angularVelocity = -turnSpeed;
    }
    else if (state & TURNING_RIGHT)
    {
        angularVelocity = turnSpeed;
    }
    else
    {
        angularVelocity = 0;
    }

    // Teleport to the other side of the screen if the character goes off-screen
    const Rectangle worldBox = {-(float)GetScreenWidth() / 2, -(float)GetScreenHeight() / 2, (float)GetScreenWidth(), (float)GetScreenHeight()};
    
    if (origin.x > worldBox.x + worldBox.width + CHARACTER_SIZE / 4)
    {
        Translate({-(float)worldBox.width - CHARACTER_SIZE / 2, 0});
    }
    else if (origin.x < worldBox.x - CHARACTER_SIZE / 4)
    {
        Translate({(float)worldBox.width + CHARACTER_SIZE / 2, 0});
    }
    if (origin.y > worldBox.y + worldBox.height + CHARACTER_SIZE / 4)
    {
        Translate({0, -(float)worldBox.height - CHARACTER_SIZE / 2});
    }
    else if (origin.y < worldBox.y - CHARACTER_SIZE / 4)
    {
        Translate({0, (float)worldBox.height + CHARACTER_SIZE / 2});
    }
    
    // if (origin.x > GetScreenWidth() + CHARACTER_SIZE / 4)
    // {
    //     Translate({-(float)GetScreenWidth() - CHARACTER_SIZE / 2, 0});
    // }
    // else if (origin.x < -CHARACTER_SIZE / 4)
    // {
    //     Translate({(float)GetScreenWidth() + CHARACTER_SIZE / 2, 0});
    // }
    // if (origin.y > GetScreenHeight() + CHARACTER_SIZE / 4)
    // {
    //     Translate({0, -(float)GetScreenHeight() - CHARACTER_SIZE / 2});
    // }
    // else if (origin.y < -CHARACTER_SIZE / 4)
    // {
    //     Translate({0, (float)GetScreenHeight() + CHARACTER_SIZE / 2});
    // }
}

void Character::Draw()
{
    // draw bullets
    for (size_t i = 0; i < bullets.size(); i++)
    {
        bullets[i].Draw();
    }

    // then draw player

    if (state & DEAD)
    {
        return;
    }

    Rectangle srcRect = GetFrameRec();

    if (state & DYING)
    {
        const float deathProgress = (GetTime() - lastDeathTime) / CHARACTER_DYING_TIME;
        const float scale = 1 + deathProgress;
        const float deathFade = 1 - deathProgress;

        this->bounds = {origin.x - scale * CHARACTER_SIZE / 2, origin.y - scale * CHARACTER_SIZE / 2,
                        scale * CHARACTER_SIZE, scale * CHARACTER_SIZE};

        DrawTexturePro(*texture, srcRect, {origin.x, origin.y, bounds.width, bounds.height},
                       {CHARACTER_SIZE * scale / 2, CHARACTER_SIZE * scale / 2}, rotation, Fade(WHITE, deathFade));
        return;
    }

    DrawTexturePro(*texture, srcRect, {origin.x, origin.y, bounds.width, bounds.height},
                   {GetBounds().width / 2, GetBounds().height / 2}, GetRotation(), WHITE);
}

void Character::DrawDebug()
{
    GameObject::DrawDebug();
    for (size_t i = 0; i < bullets.size(); i++)
    {
        bullets[i].DrawDebug();
    }
}

void Character::HandleCollision(GameObject *other, Vector2 *pushVector)
{
    GameObject::HandleCollision(other, pushVector);
}

void Character::Accelerate(float acceleration)
{
    this->velocity = Vector2Add(this->velocity, Vector2Scale(this->forwardDir, acceleration * GetFrameTime()));
}

Rectangle Character::GetFrameRec()
{
    return Rectangle{0, 0, (float)texture->width, (float)texture->height};
}

void Character::Shoot()
{
    if (!CanShoot())
    {
        return;
    }
    Vector2 bulletDir = Vector2Rotate(forwardDir, (bulletsPerShot - 1) * bulletsSpread * DEG2RAD / 2);
    for (int i = 0; i < bulletsPerShot; i++)
    {
        bullets.push_back(Bullet(Vector2Add(this->origin, Vector2Scale(this->forwardDir, CHARACTER_SIZE / 4)),
                                 Vector2Rotate(bulletDir, -i * bulletsSpread * DEG2RAD), bulletsSpeed, this->type == PLAYER));
    }
    lastShotTime = GetTime();
    PlaySound(shootSound);
}

void Character::CleanBullets()
{
    for (size_t i = 0; i < bullets.size(); i++)
    {
        if (!bullets[i].IsAlive())
        {
            bullets.erase(bullets.begin() + i);
        }
    }
    bullets.shrink_to_fit();
}

bool Character::CanBeKilled()
{
    return IsAlive();
}

bool Character::CanBeHit()
{
    return IsAlive();
}

// returns true if character is killed, false otherwise
bool Character::Kill()
{
    if ((state & (DYING | DEAD)) || this->lives <= 0)
    {
        return false;
    }
    this->lives--;
    this->state = DYING;
    this->lastDeathTime = GetTime();
    this->hitbox.clear();
    return true;
}
void Character::Respawn()
{
    // respawn in any position
    this->origin = {(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())};
    this->bounds = {origin.x - CHARACTER_SIZE / 2, origin.y - CHARACTER_SIZE / 2, CHARACTER_SIZE, CHARACTER_SIZE};
    this->rotation = 0;
    this->forwardDir = {0, -1};
    this->velocity = {0, 0};
    this->state = IDLE;
    // leave bullets live
    SetDefaultHitBox();
}

void Character::PauseSounds()
{
    PauseSound(thrustSound);
    PauseSound(shootSound);
}

void Character::ResumeSounds()
{
    ResumeSound(thrustSound);
    for (size_t i = 0; i < bullets.size(); i++)
    {
        bullets[i].ResumeSounds();
    }
}

void Character::AddLife()
{
    this->lives = fminf(this->lives + 1, CHARACTER_MAX_LIVES);
}

void Character::SetDefaultHitBox()
{
    // hitbox defaults to bounds rectangle
    hitbox = {{bounds.x, bounds.y},
              {bounds.x + bounds.width, bounds.y},
              {bounds.x + bounds.width, bounds.y + bounds.height},
              {bounds.x, bounds.y + bounds.height}};
}
