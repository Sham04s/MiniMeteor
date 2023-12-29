#include "button.hpp"
#include "resource_manager.hpp"
#include <stdio.h>

Button::Button(Rectangle relBounds, UIObject *parent, const char *text, ButtonVariant variant, ButtonSize size)
    : UIObject(relBounds, parent, nullptr)
{
    this->pressed = false;
    this->hovered = false;
    this->clicked = false;
    this->text = text;
    this->onClickFunc = nullptr;
    this->variant = variant;
    this->size = size;
    if (variant == BUTTON_SECONDARY)
    {
        this->texture = ResourceManager::GetUITexture(BUTTON_SECONDARY_TEXTURE);
    }
    else
    {
        this->texture = ResourceManager::GetUITexture(BUTTON_PRIMARY_TEXTURE);
    }
    this->textBounds = {bounds.x + BUTTON_PADDING, bounds.y + BUTTON_PADDING, bounds.width - BUTTON_PADDING * 2, bounds.height - BUTTON_PADDING * 2};
}

Button::Button(Vector2 relPos, UIObject *parent, const char *text, ButtonVariant variant, ButtonSize size, std::function<void()> onClick)
    : UIObject(Rectangle{relPos.x, relPos.y, BUTTON_WIDTH, BUTTON_HEIGHT}, parent, nullptr)
{
    this->pressed = false;
    this->hovered = false;
    this->clicked = false;
    this->text = text;
    this->onClickFunc = onClick;
    this->variant = variant;
    this->size = size;
    if (variant == BUTTON_SECONDARY)
    {
        this->texture = ResourceManager::GetUITexture(BUTTON_SECONDARY_TEXTURE);
    }
    else
    {
        this->texture = ResourceManager::GetUITexture(BUTTON_PRIMARY_TEXTURE);
    }
    this->textBounds = {bounds.x + BUTTON_PADDING, bounds.y + BUTTON_PADDING, bounds.width - BUTTON_PADDING * 2, bounds.height - BUTTON_PADDING * 2};
}

Button::~Button()
{
    UIObject::~UIObject();
}

void Button::Update()
{
    UIObject::Update();
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
        }
        else
        {
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
    unsigned int frame = 0;
    if (hovered)
    {
        frame += 2;
    }
    if (pressed)
    {
        frame += 1;
    }
    Rectangle srcRect = ResourceManager::GetUISrcRect(variant == BUTTON_SECONDARY ? BUTTON_SECONDARY_TEXTURE : BUTTON_PRIMARY_TEXTURE, frame);
    DrawTexturePro(*texture, srcRect, bounds, {0}, 0, WHITE);

    Vector2 textDst = {textBounds.x + textBounds.width / 2 - MeasureText(text, BUTTON_FONT_SIZE) / 2, textBounds.y + textBounds.height / 2 - BUTTON_FONT_SIZE / 2};
    if (pressed)
    {
        textDst.y += BUTTON_PRESSED_OFFSET;
    }
    // DrawText(text, textDst.x, textDst.y, BUTTON_FONT_SIZE, BUTTON_FONT_COLOR);
    DrawTextEx(*ResourceManager::GetFont(), text, textDst, BUTTON_FONT_SIZE, 1, BUTTON_FONT_COLOR);
}

void Button::DrawDebug()
{
    UIObject::DrawDebug();
    DrawRectangleLinesEx(bounds, 1, RED);
    DrawRectangleLinesEx(textBounds, 1, RED);
}

void Button::SetParent(UIObject *parent)
{
    UIObject::SetParent(parent);
    this->textBounds = {bounds.x + BUTTON_PADDING, bounds.y + BUTTON_PADDING, bounds.width - BUTTON_PADDING * 2, bounds.height - BUTTON_PADDING * 2};
}
