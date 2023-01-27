#include "GuiButton.h"
#include "Render.h"
#include "App.h"
#include "Audio.h"
#include "Textures.h"
#include "Log.h"

GuiButton::GuiButton(uint32 id, SDL_Rect bounds, const char* text) : GuiControl(GuiControlType::BUTTON, id)
{
	this->bounds = bounds;
	this->text = text;

	canClick = true;
	drawBasic = false;

	buttonTex = app->tex->Load("Assets/Textures/long_button.png");

}

GuiButton::~GuiButton()
{
	delete buttonTex;
}

bool GuiButton::Update(float dt)
{
	if (state != GuiControlState::DISABLED)
	{
		
		app->input->GetMousePosition(mouseX, mouseY);
		LOG("Mouse x: %d Mouse y: %d", mouseX, mouseY);
		LOG("bounds.x: %d bounds.h: %d", bounds.x, bounds.y);

		GuiControlState previousState = state;

		if (mouseX >= bounds.x && mouseX <= bounds.x + bounds.w &&
			mouseY >= bounds.y && mouseY <= bounds.y + bounds.h) {
		
			state = GuiControlState::FOCUSED;
			
			if (previousState != state) 
			{
				LOG("Change state from %d to %d", previousState, state);
			}

			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT)
			{
				state = GuiControlState::PRESSED;
			}

			// If mouse button pressed -> Generate event!
			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_UP)
			{
				NotifyObserver();
			}

		}
		else 
		{
			state = GuiControlState::NORMAL;
		}

	}

	return false;
}

bool GuiButton::Draw(Render* render)
{
	int size = bounds.w / 10;
	int x, y;

	// Draw the right button depending on state
	switch (state)
	{

	case GuiControlState::DISABLED: 
	{
		//render->DrawRectangle(bounds, 0, 0, 0, 0);
		render->DrawRectangle({ bounds.x * 2, bounds.y * 2, bounds.w * 2, bounds.h * 2 }, 200, 200, 200, 255, true, false);
	} break;

	case GuiControlState::NORMAL:
	{
		SDL_Rect rect = { 0, 0, 90, 27 };
		render->DrawTexture(buttonTex, bounds.x, bounds.y, &rect);

		x = 90; y = 27;
		//render->DrawRectangle({ bounds.x * 2, bounds.y * 2, bounds.w * 2, bounds.h * 2 }, 0, 0, 255, 255, true, false);

	} break;


	case GuiControlState::FOCUSED:
	{
		SDL_Rect rect = { 90, 0, 90, 27 };
		render->DrawTexture(buttonTex, bounds.x, bounds.y, &rect);
		x = 90; y = 27;
		//render->DrawRectangle({ bounds.x * 2, bounds.y * 2, bounds.w * 2, bounds.h * 2 }, 0, 0, 20, 255, true, false);

	} break;
	case GuiControlState::PRESSED:
	{
		SDL_Rect rect = { 180, 0, 97, 27 };
		render->DrawTexture(buttonTex, bounds.x, bounds.y, &rect);
		x = 120; y = 36;
		//render->DrawRectangle(bounds, 255, 255, 255, 200);
		//render->DrawRectangle({ bounds.x *2, bounds.y *2, bounds.w *2, bounds.h * 2 }, 0, 255, 0, 255, true, false);

	} break;

	case GuiControlState::SELECTED: //render->DrawRectangle(bounds, 0, 255, 0, 255);
		break;

	default:
		break;
	}

	//app->render->DrawText(text.GetString(), bounds.x, bounds.y, bounds.w, bounds.h, { 255,255,255 });
	app->render->TextDraw(text.GetString(), bounds.x + x / 4, bounds.y + y / 4, size);

	return false;
}