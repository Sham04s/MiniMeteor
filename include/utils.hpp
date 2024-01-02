#ifndef __UTILS_H__
#define __UTILS_H__

#include "button.hpp"
#include "ui_object.hpp"
#include <vector>

Rectangle CreateCenteredButtonRec(Button **mainMenuButtons, int numButtons);

std::vector<Vector2> getAxes(std::vector<Vector2> hitbox);
Vector2 Project(Vector2 axis, std::vector<Vector2> hitbox);
bool Overlaps(Vector2 a, Vector2 b, float *overlap);
Vector2 RandomVecOutsideScreen(float margin);

#endif // __UTILS_H__