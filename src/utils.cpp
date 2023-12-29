#include "utils.hpp"
#include "raymath.h"

Rectangle createCenteredButtonRec(Button **mainMenuButtons, int numButtons)
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

Vector2 project(Vector2 axis, std::vector<Vector2> hitbox)
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

bool overlaps(Vector2 a, Vector2 b, float *overlap)
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
    int side = GetRandomValue(0, 3);
    Vector2 pos;
    switch (side)
    {
    case 0: // top
        pos = {(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(-margin, 0)};
        break;
    case 1: // right
        pos = {(float)GetRandomValue(GetScreenWidth(), GetScreenWidth() + margin), (float)GetRandomValue(0, GetScreenHeight())};
        break;
    case 2: // bottom
        pos = {(float)GetRandomValue(0, GetScreenWidth()), (float)GetRandomValue(GetScreenHeight(), GetScreenHeight() + margin)};
        break;
    case 3: // left
        pos = {(float)GetRandomValue(-margin, 0), (float)GetRandomValue(0, GetScreenHeight())};
        break;
    default:
        break;
    }
    return pos;
}
