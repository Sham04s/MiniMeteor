#include "player.hpp"

Player::Player(Vector2 origin) : GameObject({origin.x - PLAYER_SIZE / 2, origin.y - PLAYER_SIZE / 2, PLAYER_SIZE, PLAYER_SIZE}, 0, {0, -1}, {}, PLAYER)
{
    this->lives = PLAYER_MAX_LIVES;
    this->initialOrigin = origin;
    this->playerState = IDLE;
    this->powerups = {};
    this->bullets = {};
    this->lastShotTime = 0;
    this->lastDeathTime = 0;
    this->mass = 64 * 64;
    this->texture = ResourceManager::GetSpriteTexture(PLAYER_SPRITES);

    SetDefaultHitBox();
}

Player::~Player()
{
}

void Player::Update()
{
    if (playerState != DYING && playerState != DEAD)
    {
        HandleInput();
    }

    if (playerState == ACCELERATING)
    {
        Accelerate(PLAYER_ACCELERATION);
    }
    if (playerState == EXTRA_ACCELERATING)
    {
        Accelerate(PLAYER_ACCELERATION * 2);
    }
    if (playerState == DYING && GetTime() - lastDeathTime > PLAYER_DYING_TIME)
    {
        playerState = DEAD;
    }
    if (playerState == DEAD && GetTime() - lastDeathTime > PLAYER_RESPAWN_TIME)
    {
        Respawn();
    }

    // move player
    Translate(Vector2Scale(this->velocity, GetFrameTime()));

    // decelerate player if above max speed
    if (Vector2Length(this->velocity) > PLAYER_MAX_SPEED * (playerState == EXTRA_ACCELERATING ? 2 : 1))
    {
        // decelerate
        this->velocity = Vector2Subtract(this->velocity, Vector2Scale(Vector2Normalize(this->velocity), PLAYER_ACCELERATION * 3 * GetFrameTime()));
    }

    // wrap around screen
    if (GetOrigin().x > GetScreenWidth() + PLAYER_SIZE / 4)
    {
        Translate({(float)-GetScreenWidth() - PLAYER_SIZE / 2, 0});
    }
    if (GetOrigin().x < -PLAYER_SIZE / 2)
    {
        Translate({(float)GetScreenWidth() + PLAYER_SIZE / 2, 0});
    }
    if (GetOrigin().y > GetScreenHeight() + PLAYER_SIZE / 2)
    {
        Translate({0, (float)-GetScreenHeight() - PLAYER_SIZE / 2});
    }
    if (GetOrigin().y < -PLAYER_SIZE / 2)
    {
        Translate({0, (float)GetScreenHeight() + PLAYER_SIZE / 2});
    }

    for (size_t i = 0; i < bullets.size(); i++)
    {
        bullets[i].Update();
    }

    // apply collision response
    // if (this->enteredCollision && this->lastCollisionObject->GetType() == ASTEROID)
    // {
    //     Asteroid *asteroid = (Asteroid *)this->lastCollisionObject;

    //     float u = Vector2Length(Vector2Subtract(this->velocity, asteroid->GetVelocity()));                            // relative velocity
    //     float e = 0.7f;                                                                                               // coefficient of restitution
    //     float mA = 50;                                                                                                // mass of asteroid
    //     float mP = 80;                                                                                                // mass of player
    //     float angle = Vector2Angle(Vector2Subtract(asteroid->GetOrigin(), this->lastCollisionPoint), this->velocity); // angle between asteroid and player
    //     float q = (mA / mP);                                                                                          // mass ratio

    //     float f = (1 + e) / (2 * q + 1 + pow(sin(angle), 2));

    //     float va = f * u;               // velocity of asteroid after collision
    //     float vp = (1 - 2 * q * f) * u; // velocity of player after collision

    //     float w = (f * sin(angle) * u) / Vector2Length(Vector2Subtract(asteroid->GetOrigin(), this->lastCollisionPoint)); // angular velocity of asteroid after collision

    //     asteroid->SetVelocity(Vector2Scale(Vector2Normalize(Vector2Subtract(asteroid->GetOrigin(), this->lastCollisionPoint)), va));
    //     asteroid->SetAngularVelocity(w * RAD2DEG);

    //     this->velocity = Vector2Scale(Vector2Normalize(Vector2Subtract(this->origin, this->lastCollisionPoint)), vp);
    //     this->ResetCollisionChecks();
    // }
}

void Player::HandleInput()
{
    if (IsKeyPressed(KEY_W))
    {
        this->playerState = ACCELERATING;
    }
    if (IsKeyReleased(KEY_W))
    {
        this->playerState = IDLE;
    }
    if (IsKeyPressed(KEY_LEFT_SHIFT) && this->playerState == ACCELERATING)
    {
        this->playerState = EXTRA_ACCELERATING;
    }
    if (IsKeyReleased(KEY_LEFT_SHIFT) && this->playerState == EXTRA_ACCELERATING)
    {
        this->playerState = ACCELERATING;
    }

    if (IsKeyDown(KEY_A))
    {
        Rotate(-PLAYER_TURN_SPEED * GetFrameTime());
    }
    if (IsKeyDown(KEY_D))
    {
        Rotate(PLAYER_TURN_SPEED * GetFrameTime());
    }
    if (IsKeyPressed(KEY_SPACE))
    {
        PurgeBullets();
        Shoot();
    }
}

void Player::AddLive()
{
    this->lives++; // TODO: cosider checking if lives < PLAYER_MAX_LIVES or not adding lives at all
}

void Player::AddPowerUp(/*PowerUp powerup*/)
{
    // TODO: implement
}

void Player::Draw()
{
    // draw bullets
    for (size_t i = 0; i < bullets.size(); i++)
    {
        bullets[i].Draw();
    }

    // then draw player
    int frame = 0;

    switch (playerState)
    {
    case ACCELERATING:
        frame = 1;
        break;

    case EXTRA_ACCELERATING:
        frame = 2;
        break;

    case DYING:
        frame = (int)((GetTime() - lastDeathTime) / PLAYER_DYING_TIME * 4) + 3;
        break;

    case DEAD:
        return;

    default:
        frame = 0;
        break;
    }

    // TODO: maybe make sprite for all player states
    Rectangle srcRect = ResourceManager::GetSpriteSrcRect(PLAYER_SPRITES, frame);

    DrawTexturePro(this->texture, srcRect, {origin.x, origin.y, bounds.width, bounds.height},
                   {GetBounds().width / 2, GetBounds().height / 2}, GetRotation(), WHITE);
}

void Player::DrawDebug()
{
    GameObject::DrawDebug();
    for (size_t i = 0; i < bullets.size(); i++)
    {
        bullets[i].DrawDebug();
    }
}
void Player::Kill()
{
    if (playerState == DYING || playerState == DEAD || this->lives <= 0)
    {
        return;
    }
    this->lives--;
    this->playerState = DYING;
    this->lastDeathTime = GetTime();
}

void Player::Respawn() // TODO: consider separating into Respawn() and Respaw() methods
{
    this->origin = this->initialOrigin;
    this->bounds = {origin.x - PLAYER_SIZE / 2, origin.y - PLAYER_SIZE / 2, PLAYER_SIZE, PLAYER_SIZE};
    this->rotation = 0;
    this->forwardDir = {0, -1};
    this->velocity = {0, 0};
    this->playerState = IDLE;
    this->powerups = {};
    // ResetCollisionChecks();
    // leave bullets alone
    SetDefaultHitBox();
}

bool Player::CheckCollision(GameObject *other, Vector2 *collisionPoint)
{
    return GameObject::CheckCollision(other, collisionPoint);
}

void Player::Accelerate(float acceleration)
{
    this->velocity = Vector2Add(this->velocity, Vector2Scale(this->forwardDir, acceleration * GetFrameTime()));
}

void Player::Shoot()
{
    if (GetTime() - lastShotTime < SHOOT_COOLDOWN)
    {
        return;
    }
    bullets.push_back(Bullet(this->origin, this->forwardDir)); // TODO: consider asigning bullets velocity in Bullet constructor
    lastShotTime = GetTime();
}

void Player::PurgeBullets()
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

void Player::SetDefaultHitBox()
{
    this->hitbox = {{0.0f, -0.4f}, {0.4f, 0.35f}, {-0.4f, 0.35f}}; // up, right-down, left-down

    for (size_t i = 0; i < hitbox.size(); i++)
    {
        this->hitbox[i] = Vector2Add(Vector2Scale(this->hitbox[i], PLAYER_SIZE / 2), GetOrigin());
    }
}
