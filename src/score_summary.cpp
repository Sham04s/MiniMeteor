#include "score_summary.hpp"
#include <stdio.h>

ScoreSummary::ScoreSummary(Rectangle relBounds, UIObject *parent) : UIObject(relBounds, parent, nullptr)
{
    int numLines = NUM_SCORE_TYPES + 1; // +1 for total score
    fontSize = (int)(bounds.height / numLines) - SCORE_PADDING * 2;
    if (fontSize < MIN_SCORE_FONT_SIZE)
    {
        fontSize = MIN_SCORE_FONT_SIZE;
    }

    for (int i = 0; i < NUM_SCORE_TYPES + 1; i++)
    {
        textLines[i] = new char[100];
    }
}

ScoreSummary::~ScoreSummary()
{
}

void ScoreSummary::Update()
{
    for (int i = 0; i < NUM_SCORE_TYPES; i++)
    {
        sprintf(textLines[i], "%s: %d", GetScoreName((ScoreType)i), GetRawScore((ScoreType)i));
    }
    sprintf(textLines[NUM_SCORE_TYPES], "Total: %d", GetTotalScore());
}

void ScoreSummary::Draw()
{
    for (int i = 0; i < NUM_SCORE_TYPES + 1; i++)
    {
        // DrawText(textLines[i], bounds.x, bounds.y + (fontSize + SCORE_PADDING) * i, fontSize, WHITE);
        DrawTextEx(*ResourceManager::GetFont(), textLines[i], {bounds.x, bounds.y + (fontSize + SCORE_PADDING) * i}, fontSize, 1, WHITE);
    }
    DrawLineEx({bounds.x, bounds.y + (fontSize + SCORE_PADDING) * NUM_SCORE_TYPES}, {bounds.x + bounds.width, bounds.y + (fontSize + SCORE_PADDING) * NUM_SCORE_TYPES}, 1, WHITE);
}

void ScoreSummary::DrawDebug()
{
    UIObject::DrawDebug();
}
