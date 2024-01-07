#ifndef __SCORE_REGISTRY_H__
#define __SCORE_REGISTRY_H__

#include "raylib.h"
#include "game/objects/power_up.hpp"

#include <map>

enum ScoreType{
    SHIELD_POWERUP_COLLECTED = SHIELD,
    TEMPORARY_SHIELD_POWERUP_COLLECTED = TEMPORARY_SHIELD,
    TEMPORARY_INFINITE_BOOST_POWERUP_COLLECTED = TEMPORARY_INFINITE_BOOST,
    FIRE_RATE_UPGRADE_POWERUP_COLLECTED = SHOOT_COOLDOWN_UPGRADE,
    BULLET_SPEED_UPGRADE_POWERUP_COLLECTED = BULLET_SPEED_UPGRADE,
    BULLET_SPREAD_UPGRADE_POWERUP_COLLECTED = BULLET_SPREAD_UPGRADE,
    EXTRA_BULLET_UPGRADE_POWERUP_COLLECTED = EXTRA_BULLET_UPGRADE,
    LIFE_POWERUP_COLLECTED = LIFE,
    TIME_ALIVE,
    BASIC_ENEMY_KILLED,
    SMALL_ASTEROID_DESTROYED,
    LARGE_ASTEROID_DESTROYED,
    NUM_SCORE_TYPES
};

void InitScoreRegistry();
void AddScore(ScoreType type, float multiplier);
int GetRawScore(ScoreType type);
int GetScore(ScoreType type);
int GetTotalScore();
int GetHighScore();
const char *GetScoreName(ScoreType type);
const char *GetGenericScoreName(ScoreType type);
void ResetScoreRegistry();

#endif // __SCORE_REGISTRY_H__