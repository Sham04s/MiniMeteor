#include "player.hpp"

Player::Player(Vector2 origin) : Character(origin)
{
    this->lives = 4;
    this->initialOrigin = {(float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2};
    this->state = CHARACTER_IDLE;
    this->bullets = {};
    this->lastShotTime = 0;
    this->lastDeathTime = 0;
    this->maxSpeed = 200;
    this->acceleration = 200;
    this->deceleration = 100;
    this->turnSpeed = 200;
    this->texture = ResourceManager::GetSpriteTexture(PLAYER_SPRITES);

    SetDefaultHitBox();
}

Player::~Player()
{
}

void Player::Update()
{
    Character::Update();
    
    if (state != CHARACTER_DYING && state != CHARACTER_DEAD)
    {
        HandleInput();
    }

    if (state == CHARACTER_DYING && GetTime() - lastDeathTime > CHARACTER_DYING_TIME)
    {
        state = CHARACTER_DEAD;
    }
    if (state == CHARACTER_DEAD && GetTime() - lastDeathTime > CHARACTER_RESPAWN_TIME)
    {
        Respawn();
    }
}

void Player::HandleInput()
{
    if (IsKeyPressed(KEY_W))
    {
        this->state = CHARACTER_ACCELERATING;
    }
    if (IsKeyReleased(KEY_W))
    {
        this->state = CHARACTER_IDLE;
    }
    if (IsKeyPressed(KEY_LEFT_SHIFT) && this->state == CHARACTER_ACCELERATING)
    {
        this->state = CHARACTER_EXTRA_ACCELERATING;
    }
    if (IsKeyReleased(KEY_LEFT_SHIFT) && this->state == CHARACTER_EXTRA_ACCELERATING)
    {
        this->state = CHARACTER_ACCELERATING;
    }

    if (IsKeyDown(KEY_A))
    {
        Rotate(-CHARACTER_TURN_SPEED * GetFrameTime());
    }
    if (IsKeyDown(KEY_D))
    {
        Rotate(CHARACTER_TURN_SPEED * GetFrameTime());
    }
    if (IsKeyPressed(KEY_SPACE))
    {
        Shoot();
    }
}


void Player::AddPowerup(/*PowerUp powerup*/)
{
    // TODO: implement
}

void Player::Respawn()
{
    this->origin = this->initialOrigin;
    this->bounds = {origin.x - CHARACTER_SIZE / 2, origin.y - CHARACTER_SIZE / 2, CHARACTER_SIZE, CHARACTER_SIZE};
    this->rotation = 0;
    this->forwardDir = {0, -1};
    this->velocity = {0, 0};
    this->state = CHARACTER_IDLE;
    this->powerups = {};
    // leave bullets live
    SetDefaultHitBox();
}

Rectangle Player::GetFrameRec()
{   
    int frame = 0;
    switch (state)
    {
    case CHARACTER_ACCELERATING:
        frame = 1;
        break;
    
    case CHARACTER_EXTRA_ACCELERATING:
        frame = 2;
        break;

    case CHARACTER_DYING:
        // frame = (int)((GetTime() - lastDeathTime) / CHARACTER_DYING_TIME * 4) + 3; // for sprites
        frame = 0; // idle
        break;

    default:
        frame = 0;
        break;
    }

    return ResourceManager::GetSpriteSrcRect(PLAYER_SPRITES, frame);
}

void Player::SetDefaultHitBox()
{
    this->hitbox = {{0.0f, -0.4f}, {0.4f, 0.35f}, {-0.4f, 0.35f}}; // up, right-down, left-down

    for (size_t i = 0; i < hitbox.size(); i++)
    {
        this->hitbox[i] = Vector2Add(Vector2Scale(this->hitbox[i], CHARACTER_SIZE / 2), GetOrigin());
    }
}
