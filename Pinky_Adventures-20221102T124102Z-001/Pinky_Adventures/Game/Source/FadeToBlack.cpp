#include "FadeToBlack.h"

#include "App.h"
#include "Render.h"

#include "SDL/include/SDL_render.h"
#include "Log.h"

FadeToBlack::FadeToBlack() : Module()
{
	//screenRect = {0, 0, SCREEN_WIDTH * SCREEN_SIZE, SCREEN_HEIGHT * SCREEN_SIZE};
	name.Create("FadeToBlack");
}

FadeToBlack::~FadeToBlack()
{

}

bool FadeToBlack::Awake()
{
	LOG("Loading FadeToBlack");
	bool ret = true;

	return ret;
}

bool FadeToBlack::Start()
{
	LOG("Preparing Fade Screen");

	// Enable blending mode for transparency
	SDL_SetRenderDrawBlendMode(app->render->renderer, SDL_BLENDMODE_BLEND);
	return true;
}

bool FadeToBlack::Update()
{
	// Exit this function if we are not performing a fade
	if (currentStep == Fade_Step::NONE) return true;

	if (currentStep == Fade_Step::TO_BLACK)
	{
		++frameCount;
		if (frameCount >= maxFadeFrames)
		{
			// TODO 1: Enable / disable the modules received when FadeToBlacks() gets called
			moduleToDisable->Disable();
			moduleToEnable->Enable();

			currentStep = Fade_Step::FROM_BLACK;
		}
	}
	else
	{
		--frameCount;
		if (frameCount <= 0)
		{
			currentStep = Fade_Step::NONE;
		}
	}

	return true;
}

bool FadeToBlack::PostUpdate()
{
	// NO ES LA FORMA T^T

	if (currentStep == Fade_Step::NONE) return true;

	if (currentStep == Fade_Step::TO_BLACK)
	{
		++frameCount;
		if (frameCount >= maxFadeFrames)
		{
			// TODO 1: Enable / disable the modules received when FadeToBlacks() gets called
			moduleToDisable->Disable();
			moduleToEnable->Enable();

			currentStep = Fade_Step::FROM_BLACK;
		}
	}
	else
	{
		--frameCount;
		if (frameCount <= 0)
		{
			currentStep = Fade_Step::NONE;
		}
	}


	// Exit this function if we are not performing a fade
	if (currentStep == Fade_Step::NONE) return true;

	float fadeRatio = (float)frameCount / (float)maxFadeFrames;

	// Render the black square with alpha on the screen
	SDL_SetRenderDrawColor(app->render->renderer, 0, 0, 0, (Uint8)(fadeRatio * 255.0f));
	SDL_RenderFillRect(app->render->renderer, &screenRect);

	return true;
}

bool FadeToBlack::FadingToBlack(Module* moduleToDisable, Module* moduleToEnable, float frames)
{
	bool ret = false;

	// If we are already in a fade process, ignore this call
	if(currentStep == Fade_Step::NONE)
	{
		currentStep = Fade_Step::TO_BLACK;
		frameCount = 0;
		maxFadeFrames = frames;

		// TODO 1: We need to keep track of the modules received in FadeToBlack(...)
		this->moduleToDisable = moduleToDisable;
		this->moduleToEnable = moduleToEnable;

		ret = true;
	}

	return ret;
}