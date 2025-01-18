#pragma once

#pragma once

#include "GuiControl.h"
#include "Vector2D.h"

class GuiCheckBox : public GuiControl
{
public:
    GuiCheckBox(int id, SDL_Rect bounds, const char* text);
    virtual ~GuiCheckBox();

    bool Update(float dt);
    bool Draw(std::shared_ptr<Render> render);

    bool isChecked() const { return checked; }

    bool canClick = true;
    bool drawBasic = false;
    bool isClicked = false;
    bool visible = true;
    bool checked = false;

private:

    Vector2D mousePos;

    SDL_Texture* texture = nullptr;
};

