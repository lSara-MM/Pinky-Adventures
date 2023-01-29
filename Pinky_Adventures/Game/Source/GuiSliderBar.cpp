#include "GuiSliderBar.h"
#include "Render.h"
#include "App.h"
#include "Audio.h"
#include "Textures.h"
#include "Log.h"

GuiSliderBar::GuiSliderBar(uint32 id, SDL_Rect bounds, SDL_Rect sliderBounds) : GuiControl(GuiControlType::SLIDERBAR, id)
{
	this->bounds = bounds;
	this->sliderBounds = sliderBounds;

	SliderBarTex = app->tex->Load("Assets/Textures/slider_button.png");
}

GuiSliderBar::~GuiSliderBar()
{
	delete SliderBarTex;
}

bool GuiSliderBar::Update(float dt)
{
	if (state != GuiControlState::DISABLED)
	{
		app->input->GetMousePosition(mouseX, mouseY);
		//LOG("Mouse x: %d Mouse y: %d", mouseX, mouseY);
		//LOG("bounds.x: %d bounds.h: %d", bounds.x, bounds.y);

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
				sliderBounds.x = mouseX;	//sliderBounds.y = mouseY;
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

bool GuiSliderBar::Draw(Render* render)
{
	SDL_Rect rect = { 0, 16, 38, 4 };

	render->DrawTexture(SliderBarTex, bounds.x, bounds.y, &rect);

	// Draw the right SliderBar depending on state
	switch (state)
	{

	case GuiControlState::DISABLED:
	{
		//rect = { 277, 0, 90, 27 };
		//render->DrawTexture(SliderBarTex, sliderBounds.x, sliderBounds.y, &rect);

	} break;

	case GuiControlState::NORMAL:
	{
		rect = { 0, 0, 14, 16 };
		render->DrawTexture(SliderBarTex, sliderBounds.x, sliderBounds.y, &rect);

	} break;

	case GuiControlState::FOCUSED:
	{
		//rect = { 90, 0, 90, 27 };
		//render->DrawTexture(SliderBarTex, sliderBounds.x, sliderBounds.y, &rect);
		rect = { 0, 0, 14, 16 };
		render->DrawTexture(SliderBarTex, sliderBounds.x, sliderBounds.y, &rect);

	} break;

	case GuiControlState::PRESSED:
	{
		rect = { 14, 0, 16, 16 };
		render->DrawTexture(SliderBarTex, sliderBounds.x, sliderBounds.y, &rect);

	} break;

	case GuiControlState::SELECTED:
		break;

	default:
		break;
	}
	
	return false;
}