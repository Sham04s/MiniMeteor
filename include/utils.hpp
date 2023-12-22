#ifndef __UTILS_H__
#define __UTILS_H__

#include "button.hpp"
#include "ui_object.hpp"
#include <vector>

Rectangle createCenteredButtonRec(Button **mainMenuButtons, int numButtons);

std::vector<Vector2> getAxes(std::vector<Vector2> hitbox);
Vector2 project(Vector2 axis, std::vector<Vector2> hitbox);
float getOverlap(Vector2 a, Vector2 b, float *overlap);

#endif // __UTILS_H__