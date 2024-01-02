#include "ui_object.hpp"
#include "resource_manager.hpp"

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
    if (texture == nullptr)
    {
        this->texture = ResourceManager::GetInvalidTexture();
    }
    else
    {
        this->texture = texture;
    }
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
        texture = ResourceManager::GetInvalidTexture();
    }

    DrawTexturePro(*texture, {0, 0, (float)texture->width, (float)texture->height}, bounds, {0, 0}, 0, WHITE);

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

void UIObject::AddChild(UIObject *child)
{
    child->SetParent(this);
    children.push_back(child);
}

void UIObject::RemoveChild(UIObject *child)
{
    children.erase(std::remove(children.begin(), children.end(), child), children.end());
}

void UIObject::SetRelBounds(Rectangle relBounds)
{
    if (this->relBounds.x == relBounds.x && this->relBounds.y == relBounds.y && this->relBounds.width == relBounds.width && this->relBounds.height == relBounds.height)
    {
        return;
    }
    
    this->relBounds = relBounds;
    Resize({(float)GetScreenWidth(), (float)GetScreenHeight()});
}

void UIObject::Resize(Vector2 prevScreenSize)
{
    Vector2 scale = {(float)GetScreenWidth() / prevScreenSize.x, (float)GetScreenHeight() / prevScreenSize.y};

    if (parent == nullptr)
    {
        bounds = {bounds.x * scale.x, bounds.y * scale.y, bounds.width * scale.x, bounds.height * scale.y};
    }
    else
    {
        bounds = {parent->GetBounds().x + relBounds.x * scale.x, parent->GetBounds().y + relBounds.y * scale.y, relBounds.width * scale.x, relBounds.height * scale.y};
    }

    for (size_t i = 0; i < children.size(); i++)
    {
        children[i]->Resize(prevScreenSize);
    }

    relBounds = {relBounds.x * scale.x, relBounds.y * scale.y, relBounds.width * scale.x, relBounds.height * scale.y};
}

void UIObject::SetParent(UIObject *parent)
{
    this->parent = parent;
    this->bounds = {parent->GetBounds().x + relBounds.x, parent->GetBounds().y + relBounds.y, relBounds.width, relBounds.height};
}
