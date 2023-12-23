#ifndef __SCORE_SUMMARY_H__
#define __SCORE_SUMMARY_H__

#include "ui_object.hpp"
#include "score_registry.hpp"

#define MIN_SCORE_FONT_SIZE 10
#define SCORE_PADDING 4

class ScoreSummary : public UIObject
{
private:
    char *textLines[NUM_SCORE_TYPES + 1] = {};
    int fontSize;

public:
    ScoreSummary(Rectangle relBounds, UIObject *parent = nullptr);
    ~ScoreSummary();

    void Update();
    void Draw();
    void DrawDebug();
};

#endif // __SCORE_SUMMARY_H__