#include "GuiControlButton.h"
#include "Render.h"
#include "Engine.h"
#include "Audio.h"
#include "Window.h"
#include "Physics.h"

GuiControlButton::GuiControlButton(int id, SDL_Rect bounds, const char* text) : GuiControl(GuiControlType::BUTTON, id)
{
	this->bounds = bounds;
	this->text = text;

	canClick = true;
	drawBasic = false;
}

GuiControlButton::~GuiControlButton()
{

}

bool GuiControlButton::Update(float dt)
{
	if (chargeSounds == false)
	{
		pressedFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Menu/pressed.wav");
		focusedFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Menu/focused.wav");
		chargeSounds = true;
	}
	
	if (state != GuiControlState::DISABLED)
	{

		// L16: TODO 3: Update the state of the GUiButton according to the mouse position
		Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();

		// Ajustar la posición del ratón según la escala de la ventana y la posición de la cámara
		int scale = Engine::GetInstance().window.get()->GetScale();
		mousePos.setX(mousePos.getX() + Engine::GetInstance().render->camera.x);
		mousePos.setY(mousePos.getY() + Engine::GetInstance().render->camera.y);
		SDL_Rect scaledBounds = { bounds.x * scale, bounds.y * scale, bounds.w * scale, bounds.h * scale };

		// Mensajes de depuración para verificar la posición del ratón y los límites del botón
		printf("Mouse Position: (%f, %f)\n", mousePos.getX(), mousePos.getY());
		//printf("Button Bounds: x=%d, y=%d, w=%d, h=%d\n", bounds.x, bounds.y, bounds.w, bounds.h);




		//If the position of the mouse if inside the bounds of the button 
		if (mousePos.getX() > bounds.x && mousePos.getX() < bounds.x + bounds.w && mousePos.getY() > bounds.y && mousePos.getY() < bounds.y + bounds.h) {

			//printf("Mouse is inside the button\n");
			state = GuiControlState::FOCUSED;

			if (state == GuiControlState::FOCUSED)
			{
				if (focused == false)
				{
					Engine::GetInstance().audio->PlayFx(focusedFxId);
					focused = true;
					pressed = false;
				}

			}

			if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
				state = GuiControlState::PRESSED;
				if (pressed == false)
				{
					Engine::GetInstance().audio->PlayFx(pressedFxId);
					pressed = true;
				}
			}

			if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
				NotifyObserver();
				pressed = false;
			}
		}
		else {
			state = GuiControlState::NORMAL;
			pressed = false;
			focused = false;
		}

		//L16: TODO 4: Draw the button according the GuiControl State
		switch (state)
		{
		case GuiControlState::DISABLED:
			Engine::GetInstance().render->DrawRectangle(scaledBounds, 200, 200, 200, 255, true, false);
			break;
		case GuiControlState::NORMAL:
			Engine::GetInstance().render->DrawText(text.c_str(), scaledBounds.x, scaledBounds.y, scaledBounds.w, scaledBounds.h, white);
			break;
		case GuiControlState::FOCUSED:

			Engine::GetInstance().render->DrawText((std::string("> ") + text + " <").c_str(), scaledBounds.x, scaledBounds.y, scaledBounds.w, scaledBounds.h, white);

			break;
		case GuiControlState::PRESSED:
			Engine::GetInstance().render->DrawText((std::string("> ") + text + " <").c_str(), scaledBounds.x, scaledBounds.y, scaledBounds.w, scaledBounds.h, yellow);
			break;
		}

		
	}

	return false;
}

