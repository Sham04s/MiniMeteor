#ifndef __RAYLIB_LOGO_H__
#define __RAYLIB_LOGO_H__

#include "ui/components/common/ui_object.hpp"

#define LOGO_DEFAULT_SIZE 256
#define LOGO_DEFAULT_LINE_THICKNESS 16

#define LOGO_TOP_LEFT_CORNER_TIME 2.0f
#define LOGO_TOP_LEFT_GROWING_TIME 1.0f
#define LOGO_BOTTOM_RIGHT_GROWING_TIME 1.0f
#define LOGO_MADE_WITH_SHOWING_TIME 1.4f
#define LOGO_RAYLIB_SHOWING_TIME 1.4f
#define LOGO_DONE_TIME 0.5f
#define LOGO_FADING_TIME 1.5f

/**
 * @brief Raylib logo animation based on the one from the original raysan's example
 * (see https://github.com/raysan5/raylib/blob/master/examples/shapes/shapes_logo_raylib_anim.c)
 *
 */
class RaylibLogo : public UIObject
{
private:
    float logoSize;
    float lineThickness;
    Rectangle logoRec;
    float logoAnimationTimer;
    bool done;

    float globalAlpha;
    int state;

    Rectangle topRec;
    Rectangle leftRec;
    Rectangle rightRec;
    Rectangle bottomRec;

    float fontSize;
    float raylibFontSize;
    Vector2 madeWithPos;
    Vector2 raylibPos;
    int madeWithRecLetterCount;
    int raylibRecLetterCount;

public:
    RaylibLogo();
    ~RaylibLogo();

    virtual void Update();
    virtual void Draw();
    virtual void DrawDebug();
    virtual void Resize(Vector2 prevScreenSize);

    void Reset();
    bool IsDone();
};

#endif // __RAYLIB_LOGO_H__