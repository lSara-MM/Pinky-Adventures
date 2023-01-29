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
	pCredits->creditsPath = config.attribute("settingsPath").as_string();

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
		bNum = i + 6;
		listButtons.Add((GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, bNum, buttons[i], { 25, 180 + 33 * i, 90, 27 }, 10, this, ButtonType::LARGE));
	}

	listButtons.start->next->data->state = GuiControlState::DISABLED;

	pSettings->CreateSettings(this);
	listButtons.Add(pSettings->listSettingsButtons.start->data);

	pCredits->CreateCredits(this, bNum);

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

	if (app->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN) {
		//player->ded = true;
		//app->audio->PlayFx(player->fxDeath);
		//app->fade->FadingToBlack(this, (Module*)app->loseScene, 0);
		buttonDebug = !buttonDebug;
	}

	/*if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
	{
		app->fade->FadingToBlack(this, (Module*)app->scene, 90);
		loaded = true;
	}*/


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

	if (pSettings->settings) { pSettings->OpenSettings(); }
	if (pCredits->credits) { pCredits->OpenCredits(); }

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
	pCredits->CleanUp();

	app->guiManager->CleanUp();
	return true;
}

bool IntroScene::OnGuiMouseClickEvent(GuiControl* control)
{
	LOG("Event by %d ", control->id);

	app->audio->PlayFx(control->fxControl);

	switch (control->id)
	{
	case 1:
		LOG("Button Close settings click");
		pSettings->CloseSettings();
		break;
	case 2:
		LOG("Slider music click");
		app->audio->ChangeMusicVolume(pSettings->music->volume100);
		break;
	case 3:
		LOG("Slider fx click");
		app->audio->ChangeFxVolume(pSettings->fx->volume100);
		break;
	case 4:
		LOG("Checkbox Fullscreen click");
		app->win->changeScreen = !app->win->changeScreen;
		app->win->ResizeWin();
		break;
	case 5:
		LOG("Checkbox Vsync click");
		(control->state == GuiControlState::NORMAL) ? app->render->flags = SDL_RENDERER_ACCELERATED : app->render->flags |= SDL_RENDERER_PRESENTVSYNC;
		break;
	case 6:
		LOG("Button start click");
		app->scene->continueEnabled = false;
		app->fade->FadingToBlack(this, (Module*)app->scene, 90);
		break;
	case 7:
		LOG("Button continue click");
		if (loaded) {
			app->fade->FadingToBlack(this, (Module*)app->scene, 90);
			//app->scene->continueEnabled = true;
		}
		//loaded = true;
		break;
	case 8:
		LOG("Button settings click");
		pSettings->settings = !pSettings->settings;
		if (!pSettings->settings)
		{
			pSettings->CloseSettings();
		}
		break;
	case 9:
		LOG("Button Credits click");
		pCredits->credits = !pCredits->credits;
		if (!pCredits->credits)
		{
			pCredits->CloseCredits();
		}
		break;

	case 10:
		LOG("Button Exit game click");
		exit = true;
		break;

	case 11:
		LOG("Button Close credits");
		pCredits->CloseCredits();
		break;
	}

	return true;
}
