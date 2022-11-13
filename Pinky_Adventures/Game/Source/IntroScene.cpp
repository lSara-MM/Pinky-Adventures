#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"

#include "IntroScene.h"
#include "Scene.h"
#include "FadeToBlack.h"


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

	return ret;
}

// Called before the first frame
bool IntroScene::Start()
{
	// L04: DONE 7: Set the window title with map/tileset info
	SString title("Pinky Adventures: width- %d, height- %d", app->win->GetWidth(), app->win->GetHeight());

	app->win->SetTitle(title.GetString());

	bgTexture = app->tex->Load(bgPath);
	p2sTexture = app->tex->Load("Assets/Maps/press2start.png");
	
	app->audio->PlayMusic(musicIntro, 0);
	//img = app->tex->Load("Assets/center.png");
	//img = app->tex->Load("Assets/bggatto.jpg");
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
	// L03: DONE 3: Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
	int a = 3 * app->win->GetScale();

	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		app->fade->FadingToBlack(this, (Module*)app->scene, 90);

	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		app->LoadGameRequest();


	if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		app->render->camera.y += a;

	if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		app->render->camera.y -= a;

	if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		app->render->camera.x += a;

	if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		app->render->camera.x -= a;

	if (app->input->GetKey(SDL_SCANCODE_C) == KEY_REPEAT)
	{
		app->render->camera.x = 0;
		app->render->camera.y = 0;
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
