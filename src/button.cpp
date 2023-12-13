#include "button.hpp"
#include "resource_manager.hpp"
#include <stdio.h>

Button::Button(Rectangle relBounds, UIObject *parent, const char *text) : UIObject(relBounds, parent, nullptr)
{
    this->pressed = false;
    this->hovered = false;
    this->clicked = false;
    this->disabled = false;
    this->text = text;
    this->onClickFunc = nullptr;
    this->texture = ResourceManager::GetUITexture(BUTTON_TEXTURE);
}

Button::Button(Vector2 relPos, UIObject *parent, const char *text, void (*onClick)()) : UIObject(Rectangle{relPos.x, relPos.y, BUTTON_WIDTH, BUTTON_HEIGHT}, parent, nullptr)
{
    this->pressed = false;
    this->hovered = false;
    this->clicked = false;
    this->disabled = false;
    this->text = text;
    this->onClickFunc = onClick;
    this->texture = ResourceManager::GetUITexture(BUTTON_TEXTURE);
}

Button::~Button()
{
    UIObject::~UIObject();
}

void Button::Update()
{
    UIObject::Update();
    if (disabled)
    {
        return;
    }
    hovered = CheckCollisionPointRec(GetMousePosition(), bounds);
    if (hovered)
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            pressed = true;
        }
        else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            if (pressed)
            {
                clicked = true;
                pressed = false;
                if (onClickFunc != nullptr)
                {
                    onClickFunc();
                }
            }
            pressed = false;
        } else {
            clicked = false;
        }
    }
    else
    {
        pressed = false;
    }
}

void Button::Draw()
{
    UIObject::Draw();
    if (disabled)
    {
        DrawText(text, bounds.x + BUTTON_TEXT_PADDING, bounds.y + BUTTON_TEXT_PADDING, BUTTON_FONT_SIZE, BUTTON_FONT_COLOR_DISABLED);
    }
    else
    {
        DrawText(text, bounds.x + BUTTON_TEXT_PADDING, bounds.y + BUTTON_TEXT_PADDING, BUTTON_FONT_SIZE, BUTTON_TEXT_COLOR);
    }
}

void Button::DrawDebug()
{
    UIObject::DrawDebug();
    DrawText(TextFormat("pressed: %s", pressed ? "true" : "false"), bounds.x + bounds.width + BUTTON_PADDING, bounds.y, 20, WHITE);
    DrawText(TextFormat("hovered: %s", hovered ? "true" : "false"), bounds.x + bounds.width + BUTTON_PADDING, bounds.y + 20, 20, WHITE);
    DrawText(TextFormat("clicked: %s", clicked ? "true" : "false"), bounds.x + bounds.width + BUTTON_PADDING, bounds.y + 40, 20, WHITE);
    DrawText(TextFormat("disabled: %s", disabled ? "true" : "false"), bounds.x + bounds.width + BUTTON_PADDING, bounds.y + 80, 20, WHITE);
}

void Button::OnClick(void (*onClick)())
{
    this->onClickFunc = onClick;
}
