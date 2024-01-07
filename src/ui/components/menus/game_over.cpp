#include "ui/components/menus/game_over.hpp"
#include "game/game.hpp"
#include <stdio.h>

GameOver::GameOver(Rectangle relBounds, UIObject *parent) : UIObject(relBounds, parent, nullptr)
{
    fontSize = SCORE_SUMMARY_FONT_SIZE;

    char *secondsAliveText = new char[64];
    char *enemiesKilledText = new char[64];
    char *asteroidsDestroyedText = new char[64];
    char *powerupsCollectedText = new char[64];
    char *totalScoreText = new char[64];
    char *highScoreText = new char[64];

    scoresText[0] = highScoreText;
    scoresText[1] = secondsAliveText;
    scoresText[2] = enemiesKilledText;
    scoresText[3] = asteroidsDestroyedText;
    scoresText[4] = powerupsCollectedText;
    scoresText[5] = totalScoreText;

    scoresRec = {bounds.x, bounds.y, 0, 0};
}

GameOver::~GameOver()
{
    for (int i = 0; i < 5; i++)
    {
        delete[] scoresText[i];
    }
}

void GameOver::Update()
{
    if (gameState.currentScreen != GAME_OVER)
    {
        return;
    }

    int highScore = GetHighScore();
    int secondsAlive = GetRawScore(TIME_ALIVE);
    int enemiesKilled = GetRawScore(BASIC_ENEMY_KILLED);
    int smallAsteroidsDestroyed = GetRawScore(SMALL_ASTEROID_DESTROYED) + GetRawScore(LARGE_ASTEROID_DESTROYED);
    int powerupsCollected = 0;

    for (int i = 0; i < TIME_ALIVE; i++)
    {
        powerupsCollected += GetRawScore((ScoreType)i);
    }

    if (GetTotalScore() == GetHighScore())
    {
        sprintf(scoresText[0], "New High Score: %d", highScore);
    }
    else
    {
        sprintf(scoresText[0], "High Score: %d", highScore);
    }

    sprintf(scoresText[1], "%s: %d", GetGenericScoreName(TIME_ALIVE), secondsAlive);
    sprintf(scoresText[2], "%s: %d", GetGenericScoreName(BASIC_ENEMY_KILLED), enemiesKilled);
    sprintf(scoresText[3], "%s: %d", GetGenericScoreName(SMALL_ASTEROID_DESTROYED), smallAsteroidsDestroyed);
    sprintf(scoresText[4], "%s: %d", GetGenericScoreName(SHIELD_POWERUP_COLLECTED), powerupsCollected);
    sprintf(scoresText[5], "Total: %d", GetTotalScore());

    scoresRec.width = 0;
    scoresRec.height = 0;

    for (int i = 0; i < 6; i++)
    {
        Vector2 scoreTextSize = MeasureTextEx(*ResourceManager::GetFont(), scoresText[i], fontSize, 1);
        if (scoreTextSize.x > scoresRec.width)
        {
            scoresRec.width = scoreTextSize.x;
        }
        scoresRec.height += scoreTextSize.y + SCORE_PADDING;
    }

}

void GameOver::Draw()
{
    // 0 - high score
    // 1 - seconds alive
    // 2 - enemies killed
    // 3 - asteroids destroyed
    // 4 - powerups collected
    // 5 - total score
    
    Vector2 highScoreSize = MeasureTextEx(*ResourceManager::GetFont(), scoresText[0], fontSize * 1.2f, 1);
    Vector2 highScorePos = {bounds.x + bounds.width / 2 - highScoreSize.x / 2, bounds.y};
    
    DrawTextEx(*ResourceManager::GetFont(), scoresText[0], highScorePos, highScoreSize.y, 1, GetTotalScore() == GetHighScore() ? GOLD : WHITE);
    DrawLineEx({bounds.x, highScorePos.y - SCORE_PADDING}, {bounds.x + bounds.width, highScorePos.y - SCORE_PADDING}, 1, WHITE);

    scoresRec.y = highScorePos.y + highScoreSize.y * 2 + SCORE_PADDING;
    scoresRec.x = bounds.x + bounds.width / 2 - scoresRec.width / 2;
    Vector2 scoreTextPos = {scoresRec.x, scoresRec.y};

    for (int i = 1; i < 5; i++)
    {
        DrawTextEx(*ResourceManager::GetFont(), scoresText[i], scoreTextPos, fontSize, 1, WHITE);
        scoreTextPos.y += fontSize + SCORE_PADDING;
    }

    Vector2 totalScoreSize = MeasureTextEx(*ResourceManager::GetFont(), scoresText[5], fontSize * 1.2f, 1);
    Vector2 totalScorePos = {bounds.x + bounds.width / 2 - totalScoreSize.x / 2, scoreTextPos.y + totalScoreSize.y + SCORE_PADDING};

    DrawTextEx(*ResourceManager::GetFont(), scoresText[5], totalScorePos, totalScoreSize.y, 1, SKYBLUE);
    DrawLineEx({bounds.x, totalScorePos.y + totalScoreSize.y + SCORE_PADDING}, {bounds.x + bounds.width, totalScorePos.y + totalScoreSize.y + SCORE_PADDING}, 1, WHITE);
   
}

void GameOver::DrawDebug()
{
    UIObject::DrawDebug();
    DrawRectangleLinesEx(scoresRec, 1, RED);
}

void GameOver::Resize(Vector2 prevScreenSize)
{
    UIObject::Resize(prevScreenSize);
    fontSize = fontSize * ((float)GetScreenHeight() / prevScreenSize.y);
    scoresRec.width = scoresRec.width * ((float)GetScreenWidth() / prevScreenSize.x);
    scoresRec.height = scoresRec.height * ((float)GetScreenHeight() / prevScreenSize.y);
    scoresRec.x = bounds.x + bounds.width / 2 - scoresRec.width / 2;
    scoresRec.y = bounds.y + bounds.height / 2 - scoresRec.height / 2;
}
