#include "GuiControlCheck.h"
#include "Render.h"
#include "Engine.h"
#include "Audio.h"
#include "Window.h"
#include "Textures.h"

GuiControlCheck::GuiControlCheck(int id, SDL_Rect bounds, const char* text)
    : GuiControl(GuiControlType::CHECKBOX, id)
{
    this->bounds = bounds;
    this->text = text;
}

GuiControlCheck::~GuiControlCheck()
{
    // Cleanup resources if necessary
    if (texture)
    {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

bool GuiControlCheck::Update(float dt)
{
    if (checkCreated == false)
    {
        pressedFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Menu/pressed.wav");
        focusedFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Menu/focused.wav");
        checkCreated = true;
    }

    if (state != GuiControlState::DISABLED)
    {
        Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();
        int scale = Engine::GetInstance().window.get()->GetScale();
        SDL_Rect scaledBounds = { bounds.x * scale, bounds.y * scale, bounds.w * scale, bounds.h * scale };

        if (mousePos.getX() > bounds.x && mousePos.getX() < bounds.x + bounds.w && mousePos.getY() > bounds.y && mousePos.getY() < bounds.y + bounds.h)
        {
            state = GuiControlState::FOCUSED;

            if (focused == false)
            {
                Engine::GetInstance().audio->PlayFx(focusedFxId);
                focused = true;
                pressed = false;
            }
            if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
            {
                state = GuiControlState::PRESSED;
                if (pressed == false)
                {
                    Engine::GetInstance().audio->PlayFx(pressedFxId);
                    pressed = true;
                }
            }

			// If mouse button pressed change a bool state
            if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
            {
                inCheck = !inCheck;
                NotifyObserver();
                click = true;
                pressed = false;
            }
        }
        else
        {
            state = GuiControlState::NORMAL;
            pressed = false;
            focused = false;
        }


        switch (state)
        {
        case GuiControlState::DISABLED:
            Engine::GetInstance().render->DrawRectangle(scaledBounds, 200, 200, 200, 255, true, false);
            break;
        case GuiControlState::NORMAL:
            Engine::GetInstance().render->DrawRectangle(scaledBounds, 255, 255, 255, 255, true, false);
            break;
        case GuiControlState::FOCUSED:
            Engine::GetInstance().render->DrawRectangle(scaledBounds, 0, 0, 255, 255, true, false);
            break;
        case GuiControlState::PRESSED:
            Engine::GetInstance().render->DrawRectangle(scaledBounds, 0, 255, 0, 255, true, false);
            break;
        }

        if (inCheck)
        {
            SDL_Rect checkmark = { scaledBounds.x + 5, scaledBounds.y + 5, scaledBounds.w - 10, scaledBounds.h - 10 };
            Engine::GetInstance().render->DrawRectangle(checkmark, 0, 255, 0, 255, true, false);
        }

    }

    return false;
}

