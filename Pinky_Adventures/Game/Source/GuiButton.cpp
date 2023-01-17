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

	//buttonTex = app->tex->Load("Assets/Textures/button_texture_atlas.png");

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

		if ((mouseX > bounds.x) && (mouseX < (bounds.x + bounds.w)) &&
			(mouseY > bounds.y) && (mouseY < (bounds.y + bounds.h))) {
		
			state = GuiControlState::FOCUSED;
			
			if (previousState != state) {
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
		else {
			state = GuiControlState::NORMAL;
		}

	}

	return false;
}

bool GuiButton::Draw(Render* render)
{

	// Draw the right button depending on state
	switch (state)
	{

	case GuiControlState::DISABLED: 
	{
		//render->DrawRectangle(bounds, 0, 0, 0, 0);
		render->DrawRectangle(bounds, 200, 200, 200, 255, true, false);
	} break;

	case GuiControlState::NORMAL:
	{
		//render->DrawRectangle(bounds, 255, 0, 0, 255);
		//SDL_Rect rect = {0,70,190,66};
		//render->DrawTexture(buttonTex, bounds.x, bounds.y, &rect);
		render->DrawRectangle(bounds, 0, 0, 255, 255, true, false);

	} break;


	case GuiControlState::FOCUSED:
	{
		//render->DrawRectangle(bounds, 255, 255, 255, 160);
		//SDL_Rect rect = { 0,0,190,66 };
		//render->DrawTexture(buttonTex, bounds.x, bounds.y, &rect);
		render->DrawRectangle(bounds, 0, 0, 20, 255, true, false);

	} break;
	case GuiControlState::PRESSED:
	{
		//SDL_Rect rect = { 0,141,190,66 };
		//render->DrawTexture(buttonTex, bounds.x, bounds.y, &rect);
		//render->DrawRectangle(bounds, 255, 255, 255, 200);
		render->DrawRectangle(bounds, 0, 255, 0, 255, true, false);

	} break;

	case GuiControlState::SELECTED: //render->DrawRectangle(bounds, 0, 255, 0, 255);
		break;

	default:
		break;
	}

	return false;
}