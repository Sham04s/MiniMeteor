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

    virtual void SetParent(UIObject *parent);

public:
    UIObject() : bounds({0, 0, 0, 0}), relBounds({0, 0, 0, 0}), parent(nullptr), children({}){};
    UIObject(Rectangle relBounds, UIObject *parent, Texture2D *texture);
    virtual ~UIObject();

    virtual void Update();
    virtual void Draw();
    virtual void DrawDebug();

    void AddChild(UIObject *child);
    void RemoveChild(UIObject *child);

    Rectangle GetBounds() { return bounds; }
    Rectangle GetRelBounds() { return relBounds; }
    UIObject *GetParent() { return parent; }
    std::vector<UIObject *> *GetChildren() { return &children; }

    void SetRelPos(Vector2 relPos) { relBounds.x = relPos.x; relBounds.y = relPos.y; }
    void SetRelBounds(Rectangle relBounds) { this->relBounds = relBounds; }
};
#endif // __UI_OBJECT_H__