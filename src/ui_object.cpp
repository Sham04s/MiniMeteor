#include "ui_object.hpp"

UIObject::UIObject(Rectangle relBounds, UIObject *parent, Texture2D *texture)
{
    if (parent == nullptr) // if no parent, then bounds are relative to screen
    {
        this->bounds = relBounds;
    }
    else
    {
        this->bounds = {parent->GetBounds().x + relBounds.x, parent->GetBounds().y + relBounds.y, relBounds.width, relBounds.height};
    }
    this->relBounds = relBounds;
    this->parent = parent;
    this->children = {};
    this->texture = texture;
}

UIObject::~UIObject() // deleting a UIObject also deletes its children
{
    for (size_t i = 0; i < children.size(); i++)
    {
        delete children[i];
    }
}

void UIObject::Update()
{
    for (size_t i = 0; i < children.size(); i++)
    {
        children[i]->Update();
    }
}

void UIObject::Draw()
{
    if (texture == nullptr)
    {
        Rectangle dst = {bounds.x, bounds.y, bounds.width / 2, bounds.height / 2};
        DrawRectanglePro(dst, {dst.width / 2, dst.height / 2}, 0, WHITE);
        DrawRectanglePro({dst.x + dst.width / 2, dst.y + dst.height / 2, dst.width, dst.height}, {dst.width / 2, dst.height / 2}, 0, WHITE);
        dst.y += dst.height;
        DrawRectanglePro(dst, {dst.width / 2, dst.height / 2}, 0, PURPLE);
        DrawRectanglePro({dst.x + dst.width / 2, dst.y - dst.height / 2, dst.width, dst.height}, {dst.width / 2, dst.height / 2}, 0, PURPLE);
        return;
    }
    else
    {
        DrawTexturePro(*texture, {0, 0, (float)texture->width, (float)texture->height}, bounds, {0, 0}, 0, WHITE);
    }

    for (size_t i = 0; i < children.size(); i++)
    {
        children[i]->Draw();
    }
}

void UIObject::DrawDebug()
{
    DrawRectangleLinesEx(bounds, 1, RED);
    for (size_t i = 0; i < children.size(); i++)
    {
        children[i]->DrawDebug();
    }
}
