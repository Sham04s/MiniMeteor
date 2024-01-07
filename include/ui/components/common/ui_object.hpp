#ifndef __UI_OBJECT_H__
#define __UI_OBJECT_H__

#include "raylib.h"

#include <vector>
#include <algorithm>

#define DEFAULT_FONT_SIZE 30

/**
 * @brief A UIObject is a component of the UI that can be drawn and updated.
 * Is the base class for all UI components.
 */
class UIObject
{
protected:
    /**
     * @brief Absolute bounds of the UIObject
     */
    Rectangle bounds;

    /**
     * @brief Relative bounds of the UIObject (relative to the parent)
     */
    Rectangle relBounds;

    UIObject *parent;
    std::vector<UIObject *> children;

    /**
     * @brief The texture of the UIObject
     */
    Texture2D *texture;

    virtual void SetParent(UIObject *parent);

public:
    /**
     * @brief Construct a new UIObject object with a relative position and size
     * 
     * @param relBounds The relative bounds of the UIObject
     * @param parent The parent UIObject
     * @param texture The texture of the UIObject
     */
    UIObject(Rectangle relBounds, UIObject *parent, Texture2D *texture);
    UIObject() : UIObject({0, 0, 0, 0}, nullptr, nullptr) {}
    virtual ~UIObject();

    /**
     * @brief Update the UIObject and its children
     */
    virtual void Update();

    /**
     * @brief Draw the UIObject and its children
     */
    virtual void Draw();

    /**
     * @brief Draw debug information about the UIObject and its children
     */
    virtual void DrawDebug();

    /**
     * @brief Add a child to the UIObject
     * 
     * @param child The child to add
     */
    void AddChild(UIObject *child);

    /**
     * @brief Remove a child from the UIObject
     * 
     * @param child The child to remove
     */
    void RemoveChild(UIObject *child);

    Rectangle GetBounds() { return bounds; }
    Rectangle GetRelBounds() { return relBounds; }
    UIObject *GetParent() { return parent; }
    std::vector<UIObject *> *GetChildren() { return &children; }

    /**
     * @brief Set the relative position of the UIObject (relative to the parent)
     * 
     * @param relPos The relative position of the UIObject
     */
    void SetRelPos(Vector2 relPos) { relBounds.x = relPos.x; relBounds.y = relPos.y; }

    /**
     * @brief Set the relative size of the UIObject (relative to the parent) and resize it 
     * 
     * @param relSize The relative size of the UIObject
     */
    void SetRelBounds(Rectangle relBounds);

    /**
     * @brief Resize the UIObject and its children according to the new screen size and the previous screen size
     * 
     * @param prevScreenSize The previous screen size
     */
    virtual void Resize(Vector2 prevScreenSize);
};
#endif // __UI_OBJECT_H__