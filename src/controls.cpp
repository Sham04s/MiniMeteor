#include "controls.hpp"
#include "resource_manager.hpp"

Controls::Controls()
    : Menu("Controls")
{
    this->controlsImage = ResourceManager::GetUITexture(CONTROLS_TEXTURE);
}

Controls::~Controls()
{
}

void Controls::Update()
{
    UIObject::Update();
}

void Controls::Draw()
{
    Menu::Draw();
    DrawTexturePro(*controlsImage, {0, 0, (float)controlsImage->width, (float)controlsImage->height},
                   {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, {0, 0}, 0, WHITE);
}

void Controls::DrawDebug()
{
    UIObject::DrawDebug();
}

void Controls::Resize(Vector2 prevScreenSize)
{
    UIObject::Resize(prevScreenSize);
}
