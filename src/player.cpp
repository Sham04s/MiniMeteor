#include "player.hpp"

Player::Player(Vector2 origin, int zIndex) : GameObject({origin.x - PLAYER_SIZE / 2, origin.y - PLAYER_SIZE / 2, PLAYER_SIZE, PLAYER_SIZE * 1.5f}, 0, {0, -1}, zIndex, {}, PLAYER)
{
    this->initialOrigin = origin;
    this->lives = PLAYER_MAX_LIVES;
    this->velocity = {0, 0};
    this->playerState = IDLE;
    this->powerups = {};
    this->bullets = {};
    this->lastShotTime = 0;
    this->lastDeathTime = 0;
    this->texture = ResourceManager::GetSpriteTexture(PLAYER_SPRITE);

    SetDefaultHitBox();
}

Player::~Player()
{
}

void Player::Update()
{
    if (playerState != DYING)
    {
        HandleInput();
    }

    if (playerState == ACCELERATING)
    {
        Accelerate(PLAYER_ACCELERATION);
    }
    if (playerState == BRAKING)
    {
        Decelerate(PLAYER_DECELERATION);
    }
    if (playerState == DYING && GetTime() - lastDeathTime > PLAYER_DYING_TIME)
    {
        Reset();
    }

    Translate(Vector2Scale(this->velocity, GetFrameTime()));
    // wrap around screen
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

    for (size_t i = 0; i < bullets.size(); i++)
    {
        bullets[i].Update();
    }
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
    if (IsKeyPressed(KEY_S))
    {
        this->playerState = BRAKING; // TODO: add propper braking animation
    }
    if (IsKeyReleased(KEY_S))
    {
        this->playerState = IDLE;
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
    Rectangle srcRect = ResourceManager::GetSpriteSrcRect(PLAYER_SPRITE, playerState);
    if (playerState == DYING)
    {
        // srcRect = ResourceManager::GetSpriteSrcRect(PLAYER_SPRITE, DYING); // TODO: make sprite for all player states
        srcRect = ResourceManager::GetSpriteSrcRect(PLAYER_SPRITE, IDLE);
        DrawTexturePro(GetTexture(), srcRect, {origin.x, origin.y, bounds.width, bounds.height}, {GetBounds().width / 2, GetBounds().height / 3}, GetRotation(), RED);
    }
    DrawTexturePro(GetTexture(), srcRect, {origin.x, origin.y, bounds.width, bounds.height}, {GetBounds().width / 2, GetBounds().height / 3}, GetRotation(), WHITE);
}

void Player::DrawDebug()
{
    GameObject::DrawDebug();
}
void Player::Kill()
{
    if (playerState == DYING || this->lives <= 0)
    {
        return;
    }
    this->lives--;
    this->playerState = DYING;
    this->lastDeathTime = GetTime();
}

void Player::Reset() // TODO: consider separating into Reset() and Respaw() methods
{
    this->origin = this->initialOrigin;
    this->bounds = {origin.x - PLAYER_SIZE / 2, origin.y - PLAYER_SIZE / 2, PLAYER_SIZE, PLAYER_SIZE * 1.5f};
    this->rotation = 0;
    this->forwardDir = {0, -1};
    this->velocity = {0, 0};
    this->playerState = IDLE;
    this->powerups = {};
    ResetCollisionChecks();
    // leave bullets alone
    SetDefaultHitBox();
}

void Player::Accelerate(float acceleration)
{
    this->velocity = Vector2Add(this->velocity, Vector2Scale(this->forwardDir, acceleration * GetFrameTime()));
    if (Vector2Length(this->velocity) > PLAYER_MAX_SPEED)
    {
        this->velocity = Vector2Scale(Vector2Normalize(this->velocity), PLAYER_MAX_SPEED);
    }
}

void Player::Decelerate(float deceleration)
{
    Vector2 newVelocity = Vector2Subtract(this->velocity, Vector2Scale(this->forwardDir, deceleration * GetFrameTime()));
    if (Vector2DotProduct(this->velocity, this->forwardDir) < 0 && Vector2Length(newVelocity) > 80)
    {
        return;
    }
    this->velocity = newVelocity;
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
        this->hitbox[i] = Vector2Add(Vector2Scale(this->hitbox[i], PLAYER_SIZE), GetOrigin());
    }
}
