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
    ENEMY_SHOOTER_KILLED,
    SMALL_ASTEROID_DESTROYED,
    LARGE_ASTEROID_DESTROYED,
    NUM_SCORE_TYPES
};

void InitScoreRegistry();
void AddScore(ScoreType type, float multiplier);

/**
 * @brief Gets the raw score stored in the registry for the given ScoreType
 * (without any calculations applied)
 * 
 * @param type The ScoreType to get the raw score for
 * @return int The raw score for the given ScoreType
 */
int GetRawScore(ScoreType type);

/**
 * @brief Gets the score for the given ScoreType
 * 
 * @param type The ScoreType to get the score for
 * @return int The score for the given ScoreType
 */
int GetScore(ScoreType type);

int GetTotalScore();
int GetHighScore();

/**
 * @brief Gets the name of the given ScoreType
 * 
 * @param type The ScoreType to get the name for
 * @return const char* The name of the given ScoreType 
 */
const char *GetScoreName(ScoreType type);

/**
 * @brief Gets the generic name of the given ScoreType
 * (PowerUp, Enemies, Time, etc.)
 * 
 * @param type The ScoreType to get the generic name for
 * @return const char* The generic name of the given ScoreType 
 */
const char *GetGenericScoreName(ScoreType type);

/**
 * @brief Resets the score registry but keeps the high score intact
 * 
 */
void ResetScoreRegistry();

#endif // __SCORE_REGISTRY_H__