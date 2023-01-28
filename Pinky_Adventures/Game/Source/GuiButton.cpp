#include "GuiButton.h"
#include "Render.h"
#include "App.h"
#include "Audio.h"
#include "Textures.h"
#include "Log.h"

GuiButton::GuiButton(uint32 id, SDL_Rect bounds, ButtonType bType, const char* text, int fontSize) : GuiControl(GuiControlType::BUTTON, id)
{
	this->bounds = bounds;
	this->text = text;
	this->fontSize = fontSize;

	canClick = true;
	drawBasic = false;

	(bType == ButtonType::LARGE)?buttonTex = app->tex->Load("Assets/Textures/long_button.png"): buttonTex = app->tex->Load("Assets/Textures/small_button.png");
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
	SDL_Rect rect = { 0, 0, 0, 0 };

	// Draw the right button depending on state
	switch (state)
	{

	case GuiControlState::DISABLED: 
	{
		//render->DrawRectangle({ bounds.x * 2, bounds.y * 2, bounds.w * 2, bounds.h * 2 }, 200, 200, 200, 255, true, false);
		rect = { 0, 0, 90, 27 };
		render->DrawTexture(buttonTex, bounds.x, bounds.y, &rect);
	
	} break;

	case GuiControlState::NORMAL:
	{
		rect = { 0, 0, 90, 27 };
		render->DrawTexture(buttonTex, bounds.x, bounds.y, &rect);

	} break;


	case GuiControlState::FOCUSED:
	{
		rect = { 90, 0, 90, 27 };
		render->DrawTexture(buttonTex, bounds.x, bounds.y, &rect);

	} break;
	case GuiControlState::PRESSED:
	{
		rect = { 180, 0, 97, 27 };
		render->DrawTexture(buttonTex, bounds.x, bounds.y, &rect);
		rect = { 180, 0, 120, 40 };

	} break;

	case GuiControlState::SELECTED: //render->DrawRectangle(bounds, 0, 255, 0, 255);
		break;

	default:
		break;
	}

	int size = fontSize;
	int x = rect.w - text.Length() * size / 2 - 20;
	int y = rect.h - size / 2 + 3;

	//app->render->DrawText(text.GetString(), bounds.x, bounds.y, bounds.w, bounds.h, { 255,255,255 });
	app->render->TextDraw(text.GetString(), bounds.x + x / 4, bounds.y + y / 4, size);

	return false;
}