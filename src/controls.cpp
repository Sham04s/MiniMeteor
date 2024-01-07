#include "controls.hpp"
#include "resource_manager.hpp"
#include "utils.hpp"

Controls::Controls()
    : Menu("Controls")
{
    this->controlsImage = ResourceManager::GetUITexture(CONTROLS_TEXTURE);

    this->controlsImageRec = {0, 0, (float)controlsImage->width, (float)controlsImage->height};
    this->controlsImageRec = ResizeRectWithAspectRatio(this->controlsImageRec, GetScreenWidth(), GetScreenHeight());

    // center the image
    this->controlsImageRec.x = (GetScreenWidth() - this->controlsImageRec.width) / 2;
    this->controlsImageRec.y = (GetScreenHeight() - this->controlsImageRec.height) / 2;
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
                   controlsImageRec, {0, 0}, 0, WHITE);
}

void Controls::DrawDebug()
{
    UIObject::DrawDebug();
    DrawRectangleLinesEx(controlsImageRec, 1, YELLOW);
}

void Controls::Resize(Vector2 prevScreenSize)
{
    UIObject::Resize(prevScreenSize);
    controlsImageRec = ResizeRectWithAspectRatio(controlsImageRec, GetScreenWidth(), GetScreenHeight());
    controlsImageRec.x = (GetScreenWidth() - controlsImageRec.width) / 2;
    controlsImageRec.y = (GetScreenHeight() - controlsImageRec.height) / 2;
}
