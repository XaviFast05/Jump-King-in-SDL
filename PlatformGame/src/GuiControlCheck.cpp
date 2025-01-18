#include "GuiControlCheck.h"
#include "Render.h"
#include "Engine.h"
#include "Audio.h"
#include "Textures.h"

GuiCheckBox::GuiCheckBox(int id, SDL_Rect bounds, const char* text)
    : GuiControl(GuiControlType::CHECKBOX, id), checked(false), canClick(true), drawBasic(false)
{
    this->bounds = bounds;
    this->text = text;
}

GuiCheckBox::~GuiCheckBox()
{
    // Cleanup resources if necessary
    if (texture)
    {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

bool GuiCheckBox::Update(float dt)
{
    if (state != GuiControlState::DISABLED)
    {
        mousePos = Engine::GetInstance().input->GetMousePosition();

        // Check if the mouse is over the checkbox bounds
        if (mousePos.getX() > bounds.x && mousePos.getX() < bounds.x + bounds.w && mousePos.getY() > bounds.y && mousePos.getY() < bounds.y + bounds.h)
        {
            state = GuiControlState::FOCUSED;

            // Handle mouse input
            if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
            {
                state = GuiControlState::PRESSED;
            }

            if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
            {
                checked = !checked;
                NotifyObserver();
                isClicked = true;
            }
        }
        else
        {
            state = GuiControlState::NORMAL;
        }

        // Draw the checkbox base rectangle based on state
        switch (state)
        {
        case GuiControlState::DISABLED:
            Engine::GetInstance().render->DrawRectangle(bounds, 200, 200, 200, 255, true, false);
            break;
        case GuiControlState::NORMAL:
            Engine::GetInstance().render->DrawRectangle(bounds, 255, 255, 255, 255, true, false);
            break;
        case GuiControlState::FOCUSED:
            Engine::GetInstance().render->DrawRectangle(bounds, 0, 0, 255, 255, true, false);
            break;
        case GuiControlState::PRESSED:
            Engine::GetInstance().render->DrawRectangle(bounds, 0, 255, 0, 255, true, false);
            break;
        }

        // Draw checkmark if checked
        if (checked)
        {
            SDL_Rect checkmark = { bounds.x + 5, bounds.y + 5, bounds.w - 10, bounds.h - 10 };
            Engine::GetInstance().render->DrawRectangle(checkmark, 0, 255, 0, 255, true, false);
        }

        // Draw the text label next to the checkbox
    }

    return false;
}

