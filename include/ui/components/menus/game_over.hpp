#ifndef __SCORE_SUMMARY_H__
#define __SCORE_SUMMARY_H__

#include "ui/components/common/ui_object.hpp"
#include "utils/score_registry.hpp"

#define SCORE_SUMMARY_FONT_SIZE 24
#define SCORE_PADDING 4

class GameOver : public UIObject
{
private:
    char *scoresText[6];
    float fontSize;

    Rectangle scoresRec;

public:
    GameOver(Rectangle relBounds, UIObject *parent = nullptr);
    ~GameOver();

    void Update();
    void Draw();
    void DrawDebug();

    void Resize(Vector2 prevScreenSize);
};

#endif // __SCORE_SUMMARY_H__