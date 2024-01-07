#include "utils/utils.hpp"
#include "raymath.h"

Rectangle CreateCenteredButtonRec(Button **mainMenuButtons, int numButtons)
{
    float buttonWidth = mainMenuButtons[0]->GetWidth();
    float buttonHeight = mainMenuButtons[0]->GetHeight();
    float buttonSpacing = mainMenuButtons[0]->GetPadding();
    float totalWidth = buttonWidth;
    float totalHeight = (buttonHeight + buttonSpacing * 2) * (numButtons - 1);
    Rectangle centeredMenu = {(float)GetScreenWidth() / 2 - totalWidth / 2, (float)GetScreenHeight() / 2 - totalHeight / 2,
                              totalWidth, totalHeight};
    return centeredMenu;
}

std::vector<Vector2> getAxes(std::vector<Vector2> hitbox)
{
    std::vector<Vector2> axes = {};

    for (size_t i = 0; i < hitbox.size(); i++)
    {
        // get the current vertex
        Vector2 p1 = hitbox[i];
        // get the next vertex
        Vector2 p2 = hitbox[i + 1 == hitbox.size() ? 0 : i + 1];
        // subtract the two to get the edge vector
        Vector2 edge = Vector2Subtract(p1, p2);
        // get either perpendicular vector
        Vector2 normal = Vector2Normalize({-edge.y, edge.x});
        // the perp method is just (x, y) =&gt; (-y, x) or (y, -x)
        axes.push_back(normal);
    }

    return axes;
}

Vector2 Project(Vector2 axis, std::vector<Vector2> hitbox)
{
    float min = 0;
    float max = 0;

    for (size_t i = 0; i < hitbox.size(); i++)
    {
        float dotProduct = Vector2DotProduct(axis, hitbox[i]);
        if (i == 0 || dotProduct < min)
        {
            min = dotProduct;
        }
        if (i == 0 || dotProduct > max)
        {
            max = dotProduct;
        }
    }

    return {min, max};
}

bool Overlaps(Vector2 a, Vector2 b, float *overlap)
{
    float minA = a.x;
    float maxA = a.y;
    float minB = b.x;
    float maxB = b.y;

    if (maxA >= minB && maxB >= minA)
    {
        *overlap = fmin(maxA, maxB) - fmax(minA, minB);
        return true;
    }
    return false;
}

Vector2 RandomVecOutsideScreen(float margin)
{
    // y-axis is inverted in raylib
    // x-axis is normal

    const int top = -GetScreenHeight() / 2 - margin;
    const int right = GetScreenWidth() / 2 + margin;
    const int bottom = GetScreenHeight() / 2 + margin;
    const int left = -GetScreenWidth() / 2 - margin;

    //            margin
    //           |--|
    //   top ->  ----------------------------------------  -
    //           |                                      |  | margin
    //           |  ----------------------------------  |  -
    //           |  |                                |  |
    //           |  |             VISIBLE            |  |
    //           |  |              WORLD             |  |
    //           |  |                                |  |
    //           |  ----------------------------------  |
    //           |                                      |
    // bottom -> ----------------------------------------
    //           ^                                      ^
    //           |                                      |
    //          left                                  right
    //
    // Visible world is Rectangle{-screenWidth/2, -screenHeight/2, screenWidth, screenHeight} (the center of the screen is the world's origin)

    // pick a random side and a random position on that side
    int side = GetRandomValue(0, 3);
    Vector2 pos;
    switch (side)
    {
    case 0: // top
        pos = {(float)GetRandomValue(left, right), top + margin / 2};
        break;
    case 1: // right
        pos = {right - margin / 2, (float)GetRandomValue(top, bottom)};
        break;
    case 2: // bottom
        pos = {(float)GetRandomValue(left, right), bottom - margin / 2};
        break;
    case 3: // left
        pos = {left + margin / 2, (float)GetRandomValue(top, bottom)};
        break;
    default:
        break;
    }
    return pos;
}

// "InsideScreen" refers to the visible world
Vector2 RandomVecInsideScreen(float margin)
{
    // margin is the distance from the edge of the screen
    return {(float)GetRandomValue(-GetScreenWidth() / 2 + margin, GetScreenWidth() / 2 - margin),
            (float)GetRandomValue(-GetScreenHeight() / 2 + margin, GetScreenHeight() / 2 - margin)};
}

Texture2D *GenerateStarsBackground(int width, int height, int numStars, int minRadius, int maxRadius)
{
    Texture2D starsBackground = {0};
    Image spaceBackgroundImg = GenImageColor(width, height, BLANK);

    const float minStarAlpha = 0.3f;
    const float maxStarAlpha = 0.6f;

    for (int i = 0; i < numStars; i++)
    {
        Color starColor = {255, 255, 255, (unsigned char)GetRandomValue(minStarAlpha * 255, maxStarAlpha * 255)};
        ImageDrawCircle(&spaceBackgroundImg, GetRandomValue(0, width - maxRadius * 2),
                        GetRandomValue(0, height - maxRadius * 2), GetRandomValue(minRadius, maxRadius), starColor);
    }

    starsBackground = LoadTextureFromImage(spaceBackgroundImg);
    UnloadImage(spaceBackgroundImg);
    return new Texture2D(starsBackground);
}

Rectangle ResizeRectWithAspectRatio(Rectangle rect, float newWidth, float newHeight) {
    float aspectRatio = rect.width / rect.height;
    float targetAspectRatio = newWidth / newHeight;

    if (aspectRatio > targetAspectRatio) {
        rect.width = newWidth;
        rect.height = newWidth / aspectRatio;
    } else {
        rect.width = newHeight * aspectRatio;
        rect.height = newHeight;
    }

    return rect;
}