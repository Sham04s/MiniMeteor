#include "character.hpp"

Character::Character(Vector2 origin)
    : GameObject({origin.x - CHARACTER_SIZE / 2, origin.y - CHARACTER_SIZE / 2, CHARACTER_SIZE, CHARACTER_SIZE}, 0, {0, -1}, {}, NONE)
{
    this->lives = CHARACTER_MAX_LIVES;
    this->state = CHARACTER_IDLE;
    this->bullets = {};
    this->lastShotTime = 0;
    this->lastDeathTime = 0;
    this->maxSpeed = CHARACTER_MAX_SPEED;
    this->acceleration = CHARACTER_ACCELERATION;
    this->deceleration = CHARACTER_DECELERATION;
    this->turnSpeed = CHARACTER_TURN_SPEED;
    this->texture = ResourceManager::GetDefaultTexture();

    SetDefaultHitBox();
}

Character::~Character()
{
}

void Character::Update()
{
    // if dead, do nothing
    if (state == CHARACTER_DEAD)
    {
        return;
    }

    // if dying, wait for dying animation to finish
    if (state == CHARACTER_DYING && GetTime() - lastDeathTime > CHARACTER_DYING_TIME)
    {
        state = CHARACTER_DEAD;
    }

    if (state == CHARACTER_ACCELERATING)
    {
        Accelerate(acceleration);
    }

    if (state == CHARACTER_EXTRA_ACCELERATING)
    {
        Accelerate(acceleration * 2);
    }

    // decelerate character if above max speed
    if (Vector2Length(velocity) > maxSpeed * (state == CHARACTER_EXTRA_ACCELERATING ? 2 : 1))
    {
        // decelerate
        velocity = Vector2Subtract(velocity, Vector2Scale(Vector2Normalize(velocity), deceleration * 3 * GetFrameTime()));
    }

    // rotate character
    if (state == CHARACTER_TURNING_LEFT)
    {
        Rotate(-turnSpeed * GetFrameTime());
    }
    if (state == CHARACTER_TURNING_RIGHT)
    {
        Rotate(turnSpeed * GetFrameTime());
    }

    // move character
    Translate(Vector2Scale(velocity, GetFrameTime()));

    // wrap around screen
    if (origin.x > GetScreenWidth() + CHARACTER_SIZE / 2)
    {
        Translate({-(float)GetScreenWidth() - CHARACTER_SIZE, 0});
    }
    else if (origin.x < -CHARACTER_SIZE / 2)
    {
        Translate({(float)GetScreenWidth() + CHARACTER_SIZE, 0});
    }
    if (origin.y > GetScreenHeight() + CHARACTER_SIZE / 2)
    {
        Translate({0, -(float)GetScreenHeight() - CHARACTER_SIZE});
    }
    else if (origin.y < -CHARACTER_SIZE / 2)
    {
        Translate({0, (float)GetScreenHeight() + CHARACTER_SIZE});
    }

    // update bullets
    for (size_t i = 0; i < bullets.size(); i++)
    {
        bullets[i].Update();
    }
}

void Character::Draw()
{
    // draw bullets
    for (size_t i = 0; i < bullets.size(); i++)
    {
        bullets[i].Draw();
    }

    // then draw player

    if (state == CHARACTER_DEAD)
    {
        return;
    }

    Rectangle srcRect = GetFrameRec();

    if (state == CHARACTER_DYING)
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
    bullets.push_back(Bullet(this->origin, this->forwardDir, this->type == PLAYER)); // TODO: consider asigning bullets velocity in Bullet constructor
    lastShotTime = GetTime();

    // when shooting, try to clear bullets that are out of bounds
    CleanBullets();
}

void Character::CleanBullets()
{
    for (size_t i = 0; i < bullets.size(); i++)
    {
        if (bullets[i].isOutOfBounds())
        {
            bullets.erase(bullets.begin() + i);
        }
    }
    bullets.shrink_to_fit();
}

void Character::Kill()
{
    if (state == CHARACTER_DYING || state == CHARACTER_DEAD || this->lives <= 0)
    {
        return;
    }
    this->lives--;
    this->state = CHARACTER_DYING;
    this->lastDeathTime = GetTime();
    if (this->lives <= 0)
    {
        this->hitbox = {};
    }
}
void Character::Respawn()
{
    // respawn in any position
    this->origin = {(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(0, GetScreenHeight())};
    this->bounds = {origin.x - CHARACTER_SIZE / 2, origin.y - CHARACTER_SIZE / 2, CHARACTER_SIZE, CHARACTER_SIZE};
    this->rotation = 0;
    this->forwardDir = {0, -1};
    this->velocity = {0, 0};
    this->state = CHARACTER_IDLE;
    // leave bullets live
    SetDefaultHitBox();
}

void Character::AddLife()
{
    this->lives++; // TODO: cosider checking if lives < CHARACTER_MAX_LIVES or not adding lives at all in the game loop
}

void Character::SetDefaultHitBox()
{
    // hitbox defaults to bounds rectangle
    hitbox = {{bounds.x, bounds.y},
              {bounds.x + bounds.width, bounds.y},
              {bounds.x + bounds.width, bounds.y + bounds.height},
              {bounds.x, bounds.y + bounds.height}};
}
