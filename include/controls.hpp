#ifndef __CONTROLS_H__
#define __CONTROLS_H__

#include "menu.hpp"
#include "label.hpp"

class Controls : public Menu
{
private:
    Rectangle controlsImageRec;
    Texture2D *controlsImage;
    
public:
    Controls();
    ~Controls();

    void Update();
    void Draw();
    void DrawDebug();

    void Resize(Vector2 prevScreenSize);
};

#endif // __CONTROLS_H__