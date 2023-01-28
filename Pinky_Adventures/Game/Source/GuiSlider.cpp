#include "GuiSlider.h"
#include "Render.h"
#include "App.h"
#include "Audio.h"
#include "Textures.h"
#include "Log.h"

GuiSlider::GuiSlider(uint32 id, SDL_Rect bounds) : GuiControl(GuiControlType::SLIDER, id)
{
	this->bounds = bounds;

	SliderTex = app->tex->Load("Assets/Textures/small_Slider.png");
}

GuiSlider::~GuiSlider()
{
	delete SliderTex;
}

bool GuiSlider::Update(float dt)
{
	if (state != GuiControlState::DISABLED)
	{
		app->input->GetMousePosition(mouseX, mouseY);
		//LOG("Mouse x: %d Mouse y: %d", mouseX, mouseY);
		//LOG("bounds.x: %d bounds.h: %d", bounds.x, bounds.y);

		GuiControlState previousState = state;

		if (mouseX >= bounds.x && mouseX <= bounds.x + bounds.w &&
			mouseY >= bounds.y && mouseY <= bounds.y + bounds.h) {

			//state = GuiControlState::FOCUSED;

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

bool GuiSlider::Draw(Render* render)
{
	SDL_Rect rect = { 0, 0, 0, 0 };

	//if (SliderType == SliderType::LONG)
	//{
	//	// Draw the right Slider depending on state
	//	switch (state)
	//	{

	//	case GuiControlState::DISABLED:
	//	{
	//		rect = { 277, 0, 90, 27 };
	//		render->DrawTexture(SliderTex, bounds.x, bounds.y, &rect);

	//	} break;

	//	case GuiControlState::NORMAL:
	//	{
	//		rect = { 0, 0, 90, 27 };
	//		render->DrawTexture(SliderTex, bounds.x, bounds.y, &rect);

	//	} break;


	//	case GuiControlState::FOCUSED:
	//	{
	//		rect = { 90, 0, 90, 27 };
	//		render->DrawTexture(SliderTex, bounds.x, bounds.y, &rect);

	//	} break;
	//	case GuiControlState::PRESSED:
	//	{
	//		rect = { 180, 0, 97, 27 };
	//		render->DrawTexture(SliderTex, bounds.x, bounds.y, &rect);
	//		rect = { 180, 0, 120, 40 };

	//	} break;

	//	case GuiControlState::SELECTED: //render->DrawRectangle(bounds, 0, 255, 0, 255);
	//		break;

	//	default:
	//		break;
	//	}
	//	
	//	offsetX = 20;	offsetY = 3;
	//}
	//
	//if (SliderType == SliderType::SMALL)
	//{
	//	// Draw the right Slider depending on state
	//	switch (state)
	//	{

	//	case GuiControlState::DISABLED:
	//	{
	//		//render->DrawRectangle({ bounds.x * 2, bounds.y * 2, bounds.w * 2, bounds.h * 2 }, 200, 200, 200, 255, true, false);
	//		rect = { 85, 0, 26, 28 };
	//		render->DrawTexture(SliderTex, bounds.x, bounds.y, &rect);

	//	} break;

	//	case GuiControlState::NORMAL:
	//	{
	//		rect = { 0, 0, 26, 28 };
	//		render->DrawTexture(SliderTex, bounds.x, bounds.y, &rect);

	//	} break;


	//	case GuiControlState::FOCUSED:
	//	{
	//		rect = { 26, 0, 26, 28 };
	//		render->DrawTexture(SliderTex, bounds.x, bounds.y, &rect);

	//	} break;

	//	case GuiControlState::PRESSED:
	//	{
	//		rect = { 52, 0, 33, 28 };
	//		render->DrawTexture(SliderTex, bounds.x, bounds.y, &rect);
	//		rect = { 0, 0, 56, 41 };

	//	} break;

	//	case GuiControlState::SELECTED: //render->DrawRectangle(bounds, 0, 255, 0, 255);
	//		break;

	//	default:
	//		break;
	//	}
	//}

	return false;
}