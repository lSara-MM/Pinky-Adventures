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

	//exemple bot�
	uint w, h;
	app->win->GetWindowSize(w, h);
	button1 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "Button 1", { (int)w / 2 - 50,(int)h / 2 - 30,100,20 }, this);
	button2 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "Button 2", { (int)w / 2 - 50,(int)h / 2,100,20 }, this);

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

	if (v_start < 100)
	{
		app->render->DrawTexture(p2sTexture, 100, 350);
	}

	if (v_start == 150)
		v_start = 0;
	
	v_start++;

	app->render->TextDraw("holis", 150, 150, 15, { 255, 0, 255 });

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
