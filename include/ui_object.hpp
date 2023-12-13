#ifndef __UI_OBJECT_H__
#define __UI_OBJECT_H__

#include "raylib.h"

#include <vector>
#include <algorithm>

class UIObject
{
protected:
    Rectangle bounds;
    Rectangle relBounds; // relative to parent
    UIObject *parent;
    std::vector<UIObject *> children;
    Texture2D *texture;

public:
    UIObject() : bounds({0, 0, 0, 0}), relBounds({0, 0, 0, 0}), parent(nullptr), children({}){};
    UIObject(Rectangle relBounds, UIObject *parent, Texture2D *texture);
    virtual ~UIObject();

    virtual void Update();
    virtual void Draw();
    virtual void DrawDebug();

    void AddChild(UIObject *child) { children.push_back(child); }
    void RemoveChild(UIObject *child) { children.erase(std::remove(children.begin(), children.end(), child), children.end()); };

    Rectangle GetBounds() { return bounds; }
    Rectangle GetRelBounds() { return relBounds; }
    UIObject *GetParent() { return parent; }
    std::vector<UIObject *> *GetChildren() { return &children; }
};
#endif // __UI_OBJECT_H__