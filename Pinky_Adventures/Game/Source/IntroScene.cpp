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
	settingsPath = config.attribute("settingsPath").as_string();

	return ret;
}

// Called before the first frame
bool IntroScene::Start()
{
	SString title("Pinky Adventures: width- %d, height- %d", app->win->GetWidth(), app->win->GetHeight());

	app->win->SetTitle(title.GetString());

	bgTexture = app->tex->Load(bgPath);
	settingsTexture = app->tex->Load(settingsPath);
	
	app->audio->PlayMusic(musicIntro, 0);
	loaded = false;

	// buttons
	for (int i = 0; buttons[i] != "\n"; i++)
	{
		listButtons.Add((GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, i + 1, buttons[i], { 30, 200 + 35 * i, 90, 27 }, 10, this, ButtonType::LONG));
	}

	settings = false;
	open = false;

	listButtons.start->next->data->state = GuiControlState::DISABLED;


	// settings buttons
	bNum++;
	GuiButton* button = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, bNum, "x", { 137, 56, 26, 28 }, 10, this, ButtonType::SMALL);
	button->state = GuiControlState::NONE;
	listButtons.Add(button);
	listSettingsButtons.Add(button);

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

	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
	{
		app->fade->FadingToBlack(this, (Module*)app->scene, 90);
		loaded = true;
	}
	

	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		app->fade->FadingToBlack(this, (Module*)app->scene, 90);

	
	return true;
}

// Called each loop iteration
bool IntroScene::PostUpdate()
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	app->render->DrawTexture(bgTexture, 0, 0);

	if (settings)
	{
		OpenSettings();
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
	app->tex->UnLoad(settingsTexture);

	listButtons.Clear();

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
		app->fade->FadingToBlack(this, (Module*)app->scene, 90);
		loaded = true;
		break;
	case 3:
		LOG("Button settings click");
		settings = !settings;
		break;
	case 4:
		LOG("Button Credits click");
		app->fade->FadingToBlack(this, (Module*)app->scene, 90);
		break;

	case 5:
		LOG("Button Close settings click");
		CloseSettings();
		break;
	}

	return true;
}

bool IntroScene::OpenSettings()
{
	SDL_Rect rect = {0, 0, 226, 261};
	app->render->DrawTexture(settingsTexture, 150, 70, &rect);
	
	if (!open)
	{
		for (ListItem<GuiButton*>* i = listSettingsButtons.start; i != nullptr; i = i->next)
		{
			i->data->state = GuiControlState::NORMAL;
		}
		open = true;
	}
	
	return true;
}

bool IntroScene::CloseSettings()
{
	settings = false;
	open = false;
	for (ListItem<GuiButton*>* i = listSettingsButtons.start; i != nullptr; i = i->next)
	{
		i->data->state = GuiControlState::NONE;
	}

	return true;
}