#include "utils/score_registry.hpp"

std::map<ScoreType, int> scoreValues = {
    {TIME_ALIVE, 100},
    {BASIC_ENEMY_KILLED, 2000},
    {SMALL_ASTEROID_DESTROYED, 800},
    {LARGE_ASTEROID_DESTROYED, 1600},
    {LIFE_POWERUP_COLLECTED, 100},
    {SHIELD_POWERUP_COLLECTED, 50},
    {TEMPORARY_SHIELD_POWERUP_COLLECTED, 50},
    {TEMPORARY_INFINITE_BOOST_POWERUP_COLLECTED, 50},
    {FIRE_RATE_UPGRADE_POWERUP_COLLECTED, 50},
    {BULLET_SPEED_UPGRADE_POWERUP_COLLECTED, 50},
    {BULLET_SPREAD_UPGRADE_POWERUP_COLLECTED, 50},
    {EXTRA_BULLET_UPGRADE_POWERUP_COLLECTED, 50},
};

std::map<ScoreType, const char *> scoreNames = {
    {TIME_ALIVE, "Seconds Alive"},
    {BASIC_ENEMY_KILLED, "Basic Enemies Killed"},
    {SMALL_ASTEROID_DESTROYED, "Small Asteroids Destroyed"},
    {LARGE_ASTEROID_DESTROYED, "Large Asteroids Destroyed"},
    {LIFE_POWERUP_COLLECTED, "Lives Collected"},
    {SHIELD_POWERUP_COLLECTED, "Shields Collected"},
    {TEMPORARY_SHIELD_POWERUP_COLLECTED, "Temporary Shields Collected"},
    {TEMPORARY_INFINITE_BOOST_POWERUP_COLLECTED, "Temporary Infinite Boosts Collected"},
    {FIRE_RATE_UPGRADE_POWERUP_COLLECTED, "Fire Rate Upgrades Collected"},
    {BULLET_SPEED_UPGRADE_POWERUP_COLLECTED, "Bullet Speed Upgrades Collected"},
    {BULLET_SPREAD_UPGRADE_POWERUP_COLLECTED, "Bullet Spread Upgrades Collected"},
    {EXTRA_BULLET_UPGRADE_POWERUP_COLLECTED, "Extra Bullet Upgrades Collected"},
};

std::map<ScoreType, float> scoreRegistry;

float highScore = 0;
float totalScore = 0;

void InitScoreRegistry()
{
    totalScore = 0;
    for (auto &score : scoreValues)
    {
        scoreRegistry[score.first] = 0;
    }
}

void AddScore(ScoreType type, float multiplier)
{
    if (type == TIME_ALIVE)
    {
        scoreRegistry[TIME_ALIVE] += 1.0f * GetFrameTime() * multiplier;
        totalScore += scoreValues[type] * GetFrameTime() * multiplier;
    }
    else
    {
        scoreRegistry[type] += 1.0f * multiplier;
        totalScore += scoreValues[type] * multiplier;
    }

    highScore = totalScore > highScore ? totalScore : highScore;
}

int GetRawScore(ScoreType type)
{
    return scoreRegistry[type];
}

int GetScore(ScoreType type)
{
    return scoreRegistry[type] * scoreValues[type];
}

int GetTotalScore()
{
    return totalScore;
}

int GetHighScore()
{
    return highScore;
}

const char *GetScoreName(ScoreType type)
{
    return scoreNames[type];
}

const char *GetGenericScoreName(ScoreType type)
{
    if (type < TIME_ALIVE)
        return "Powerups Collected";
    if (type == TIME_ALIVE)
        return "Seconds Alive";
    if (type == BASIC_ENEMY_KILLED)
        return "Enemies Killed";
    if (type == SMALL_ASTEROID_DESTROYED || type == LARGE_ASTEROID_DESTROYED)
        return "Asteroids Destroyed";

    return "Unknown Score Type";
}

void ResetScoreRegistry()
{
    InitScoreRegistry();
}