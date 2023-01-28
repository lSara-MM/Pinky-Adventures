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
	p2sPath = config.attribute("p2sPath").as_string();
	return ret;
}

// Called before the first frame
bool IntroScene::Start()
{
	SString title("Pinky Adventures: width- %d, height- %d", app->win->GetWidth(), app->win->GetHeight());

	app->win->SetTitle(title.GetString());

	bgTexture = app->tex->Load(bgPath);
	p2sTexture = app->tex->Load(p2sPath);
	
	app->audio->PlayMusic(musicIntro, 0);
	loaded = false;


	// buttons
	for (int i = 0; buttons[i] != "\n"; i++)
	{
		listButtons.Add((GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, i + 1, buttons[i], { 30, 200 + 35 * i, 90, 27 }, 10, this, ButtonType::LARGE));
	}

	/*button_play = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "Play", { 30, y, 100, 100 }, this);
	button_continue = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "Continue", { 30, 250, 100, 100 }, this);
	button_options = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "Options", { 30, 300, 100, 100 }, this);
	button_credits = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "Credits", { 30, 350, 100, 100 }, this);*/

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
	

	ListItem<GuiButton*>* b = listButtons.start;

	// Start
	if (b->data->state == GuiControlState::PRESSED)
	{
		app->fade->FadingToBlack(this, (Module*)app->scene, 90);
	}

	b = b->next;

	// Continue		// faria falta comprobar que hi ha un joc guardat
	if (b->data->state == GuiControlState::PRESSED)
	{
		app->fade->FadingToBlack(this, (Module*)app->scene, 90);
		loaded = true;
	}

	b = b->next;

	// Options		
	if (b->data->state == GuiControlState::PRESSED)
	{
		// crear menu options
	}

	b = b->next;

	// Credits		
	if (b->data->state == GuiControlState::PRESSED)
	{
		app->fade->FadingToBlack(this, (Module*)app->scene, 90);
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
	
	/*
	if (v_start < 100)
	{
		app->render->DrawTexture(p2sTexture, 100, 350);
	}

	if (v_start == 150)
		v_start = 0;
	
	v_start++;*/

	app->guiManager->Draw();

	return ret;
}

// Called before quitting
bool IntroScene::CleanUp()
{
	LOG("Freeing IntroScene");
	app->audio->PauseMusic();

	app->tex->UnLoad(bgTexture);
	app->tex->UnLoad(p2sTexture);

	listButtons.Clear();

	return true;
}

bool IntroScene::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	LOG("Event by %d ", control->id);

	switch (control->id)
	{
	case 1:
		LOG("Button 1 click");
		break;
	case 2:
		LOG("Button 2 click");
		break;
	}

	return true;
}
