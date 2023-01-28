#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"

#include "IntroScene.h"
#include "Scene.h"
#include "FadeToBlack.h"
#include "GuiManager.h"


#include "Defs.h"
#include "Log.h"

IntroScene::IntroScene() : Module()
{
	name.Create("introScene");
}

// Destructor
IntroScene::~IntroScene()
{}

// Called before render is available
bool IntroScene::Awake(pugi::xml_node& config)
{
	LOG("Loading IntroScene");
	bool ret = true;

	// iterate all objects in the IntroScene
	// Check https://pugixml.org/docs/quickstart.html#access
	bgPath = config.attribute("background").as_string();
	musicIntro = config.attribute("audioIntroPath").as_string();

	
	pSettings->settingsPath = config.attribute("settingsPath").as_string();

	return ret;
}

// Called before the first frame
bool IntroScene::Start()
{
	SString title("Pinky Adventures: width- %d, height- %d", app->win->GetWidth(), app->win->GetHeight());

	app->win->SetTitle(title.GetString());

	bgTexture = app->tex->Load(bgPath);

	app->audio->PlayMusic(musicIntro, 0);
	/*loaded = false;*/

	// buttons
	for (int i = 0; buttons[i] != "\n"; i++)
	{
		listButtons.Add((GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, i + 1, buttons[i], { 25, 180 + 33 * i, 90, 27 }, 10, this, ButtonType::LONG));
		bNum = i + 1;
	}

	listButtons.start->next->data->state = GuiControlState::DISABLED;

	pSettings->GUI_id = bNum;
	pSettings->CreateSettings(this);
	listButtons.Add(pSettings->listSettingsButtons.start->data);

	exit = false;

	return true;
}

// Called each loop iteration
bool IntroScene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool IntroScene::Update(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		app->fade->FadingToBlack(this, (Module*)app->scene, 5);

	if (loaded)
	{
		listButtons.start->next->data->state = GuiControlState::NORMAL;
	}

	//if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
	//{
	//	listButtons.start->next->data->state = GuiControlState::NORMAL;
	//	loaded = true;
	//}


	/*if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		app->fade->FadingToBlack(this, (Module*)app->scene, 90);*/


	return true;
}

// Called each loop iteration
bool IntroScene::PostUpdate()
{
	bool ret = true;

	if (exit) return false;

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	app->render->DrawTexture(bgTexture, 0, 0);

	if (pSettings->settings)
	{
		pSettings->OpenSettings();
	}

	app->guiManager->Draw();

	return ret;
}

// Called before quitting
bool IntroScene::CleanUp()
{
	LOG("Freeing IntroScene");
	app->audio->PauseMusic();

	app->tex->UnLoad(bgTexture);
	
	listButtons.Clear();
	pSettings->CleanUp();

	app->guiManager->CleanUp();
	return true;
}

bool IntroScene::OnGuiMouseClickEvent(GuiControl* control)
{
	LOG("Event by %d ", control->id);

	switch (control->id)
	{
	case 1:
		LOG("Button start click");
		app->fade->FadingToBlack(this, (Module*)app->scene, 90);
		break;
	case 2:
		LOG("Button continue click");
		if (loaded) {
			app->LoadGameRequest();
		}
		//loaded = true;
		break;
	case 3:
		LOG("Button settings click");
		pSettings->settings = !pSettings->settings;
		if (!pSettings->settings)
		{
			pSettings->CloseSettings();
		}
		break;
	case 4:
		LOG("Button Credits click");
		app->fade->FadingToBlack(this, (Module*)app->scene, 90);
		break;

	case 5:
		LOG("Button Exit game click");
		exit = true;
		break;

	case 6:
		LOG("Button Close settings click");
		pSettings->CloseSettings();
		break;

	case 7:
		LOG("Slider music click");

		break;
	case 8:
		LOG("Slider sfx click");

		break;
	case 9:
		LOG("Checkbox Fullscreen click");
		app->win->changeScreen = !app->win->changeScreen;
		app->win->ResizeWin();
		break;
	case 10:
		LOG("Checkbox Vsync click");
		(control->state == GuiControlState::NORMAL) ? app->render->flags = SDL_RENDERER_ACCELERATED : app->render->flags |= SDL_RENDERER_PRESENTVSYNC;
		break;
	}

	return true;
}
