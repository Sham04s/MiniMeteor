/***********************************************************************************************
 *
 *      raylib logo animation based on original raysan's logo animation
 *      available at:
 *      https://github.com/raysan5/raylib/blob/master/examples/shapes/shapes_logo_raylib_anim.c
 *
 * ********************************************************************************************/

#include "ui/components/menus/raylib_logo.hpp"
#include "utils/resource_manager.hpp"
#include <math.h>
#include "raymath.h"

RaylibLogo::RaylibLogo()
    : UIObject({0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, nullptr, ResourceManager::GetDefaultTexture())
{
    this->logoSize = LOGO_DEFAULT_SIZE;
    this->fontSize = 32;
    this->raylibFontSize = 50;
    this->lineThickness = LOGO_DEFAULT_LINE_THICKNESS;
    Reset();
}

RaylibLogo::~RaylibLogo()
{
}

void RaylibLogo::Update()
{
    if (done)
        return;

    if (state < 5 && (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ESCAPE) ||
                      IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
    {
        state = 5;
        logoAnimationTimer = 0;
        topRec.width = logoRec.width;
        topRec.height = lineThickness;
        leftRec.width = lineThickness;
        leftRec.height = logoRec.height;
        bottomRec.width = logoRec.width;
        bottomRec.height = lineThickness;
        rightRec.width = lineThickness;
        rightRec.height = logoRec.height;
        madeWithRecLetterCount = 9;
        raylibRecLetterCount = 6;
    }

    logoAnimationTimer += GetFrameTime();
    if (state == 0) // top-left corner blinking
    {
        if ((int)(logoAnimationTimer * 4) % 2)
        {
            topRec.width = lineThickness;
            topRec.height = lineThickness;
        }
        else
        {
            topRec.width = 0;
            topRec.height = 0;
        }

        if (logoAnimationTimer >= LOGO_TOP_LEFT_CORNER_TIME)
        {
            logoAnimationTimer = 0;
            state++;
            topRec.width = lineThickness;
            topRec.height = lineThickness;
            leftRec.width = lineThickness;
            leftRec.height = lineThickness;
        }
    }
    else if (state == 1) // top and left lines growing
    {
        topRec.width = fminf(topRec.width + GetFrameTime() * logoSize / LOGO_TOP_LEFT_GROWING_TIME, logoSize);
        leftRec.height = fminf(leftRec.height + GetFrameTime() * logoSize / LOGO_TOP_LEFT_GROWING_TIME, logoSize);

        if (logoAnimationTimer >= LOGO_TOP_LEFT_GROWING_TIME)
        {
            logoAnimationTimer = 0;
            state++;
            bottomRec.width = lineThickness;
            bottomRec.height = lineThickness;
            rightRec.width = lineThickness;
            rightRec.height = lineThickness;
        }
    }
    else if (state == 2) // bottom and right growing
    {
        bottomRec.width = fminf(bottomRec.width + GetFrameTime() * logoSize / LOGO_BOTTOM_RIGHT_GROWING_TIME, logoSize);
        rightRec.height = fminf(rightRec.height + GetFrameTime() * logoSize / LOGO_BOTTOM_RIGHT_GROWING_TIME, logoSize);

        if (logoAnimationTimer >= LOGO_BOTTOM_RIGHT_GROWING_TIME)
        {
            logoAnimationTimer = 0;
            state++;
        }
    }
    else if (state == 3) // "Made with" showing
    {
        madeWithRecLetterCount = (int)Lerp(0, 9, logoAnimationTimer / LOGO_MADE_WITH_SHOWING_TIME);
        if (madeWithRecLetterCount == 4)
            madeWithRecLetterCount = 5;

        if (logoAnimationTimer >= LOGO_MADE_WITH_SHOWING_TIME)
        {
            logoAnimationTimer = 0;
            state++;
        }
    }
    else if (state == 4) // "raylib" showing
    {
        raylibRecLetterCount = (int)Lerp(0, 6, logoAnimationTimer / LOGO_RAYLIB_SHOWING_TIME);
        if (logoAnimationTimer >= LOGO_RAYLIB_SHOWING_TIME)
        {
            logoAnimationTimer = 0;
            state++;
        }
    }
    else if (state == 5) // done
    {
        if (logoAnimationTimer >= LOGO_DONE_TIME)
        {
            logoAnimationTimer = 0;
            state++;
        }
    }
    else if (state == 6) // fading out
    {
        globalAlpha = Lerp(1.0f, 0.0f, logoAnimationTimer / LOGO_FADING_TIME);

        if (logoAnimationTimer >= LOGO_FADING_TIME)
        {
            logoAnimationTimer = 0;
            done = true;
        }
    }
}

void RaylibLogo::Draw()
{
    if (done)
        return;

    DrawRectangle(0, 0, (float)GetScreenWidth(), (float)GetScreenHeight(), Fade(RAYWHITE, globalAlpha));
    DrawRectangleRec(logoRec, Fade(RAYWHITE, globalAlpha));

    DrawRectangleRec(topRec, Fade(BLACK, globalAlpha));

    Rectangle left = leftRec;
    left.y += lineThickness;
    left.height -= lineThickness;
    DrawRectangleRec(left, Fade(BLACK, globalAlpha));

    Rectangle bottom = bottomRec;
    bottom.x += lineThickness;
    bottom.width -= lineThickness;
    DrawRectangleRec(bottom, Fade(BLACK, globalAlpha));

    Rectangle right = rightRec;
    right.y += lineThickness;
    right.height -= lineThickness * 2;
    DrawRectangleRec(right, Fade(BLACK, globalAlpha));

    DrawText(TextSubtext("Made with", 0, madeWithRecLetterCount), madeWithPos.x, madeWithPos.y, fontSize, Fade(BLACK, globalAlpha));
    DrawText(TextSubtext("raylib", 0, raylibRecLetterCount), raylibPos.x, raylibPos.y, raylibFontSize, Fade(BLACK, globalAlpha));
}

void RaylibLogo::DrawDebug()
{
    UIObject::DrawDebug();
    DrawRectangleLinesEx(logoRec, 1, RED);
    DrawRectangleLinesEx(topRec, 1, RED);
    DrawRectangleLinesEx(leftRec, 1, RED);
    DrawRectangleLinesEx(bottomRec, 1, RED);
    DrawRectangleLinesEx(rightRec, 1, RED);

    DrawText(TextFormat("state: %d", state), 0, 0, 20, RED);
}

void RaylibLogo::Resize(Vector2 prevScreenSize)
{
    UIObject::Resize(prevScreenSize);

    float scale = GetScreenHeight() / prevScreenSize.y;

    logoSize = logoSize * scale;
    fontSize = fontSize * scale;
    raylibFontSize = raylibFontSize * scale;
    lineThickness = lineThickness * scale;

    logoRec = {(float)GetScreenWidth() / 2 - logoSize / 2, (float)GetScreenHeight() / 2 - logoSize / 2, logoSize, logoSize};
    if (state == 0) // starting animation
    {
        topRec = {logoRec.x, logoRec.y, 0, 0};
        leftRec = {logoRec.x, logoRec.y, 0, 0};
        bottomRec = {logoRec.x, logoRec.y + logoRec.height - lineThickness, 0, 0};
        rightRec = {logoRec.x + logoRec.width - lineThickness, logoRec.y, 0, 0};
    }
    else // is animating
    {
        topRec = {logoRec.x, logoRec.y, topRec.width * scale, topRec.height * scale};
        leftRec = {logoRec.x, logoRec.y, leftRec.width * scale, leftRec.height * scale};
        bottomRec = {logoRec.x, logoRec.y + logoRec.height - lineThickness, bottomRec.width * scale, bottomRec.height * scale};
        rightRec = {logoRec.x + logoRec.width - lineThickness, logoRec.y, rightRec.width * scale, rightRec.height * scale};
    }

    madeWithPos = {logoRec.x, logoRec.y - fontSize};
    raylibPos = {logoRec.x + logoRec.width - lineThickness * 2 - MeasureText("raylib", raylibFontSize),
                 logoRec.y + logoRec.height - 2.5f * lineThickness - raylibFontSize};
}

void RaylibLogo::Reset()
{
    this->done = false;
    this->state = 0;
    this->globalAlpha = 1.0f;
    this->logoAnimationTimer = 0;
    this->madeWithRecLetterCount = 0;
    this->raylibRecLetterCount = 0;

    Resize({(float)GetScreenWidth(), (float)GetScreenHeight()});
}

bool RaylibLogo::IsDone()
{
    return done;
}
