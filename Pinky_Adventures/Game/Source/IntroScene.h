#ifndef __INTROSCENE_H__
#define __INTROSCENE_H__

#include "Module.h"
#include "Player.h"
#include "ItemCoin.h"
#include "ItemGem.h"
#include "GuiButton.h"

struct SDL_Texture;

class IntroScene : public Module
{
public:

	IntroScene();

	// Destructor
	virtual ~IntroScene();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool OnGuiMouseClickEvent(GuiControl* control);

public:

	Player* player;
	bool loaded;

private:

	const char* musicIntro;
	const char* imagePath;

	SDL_Texture* bgTexture;
	const char* bgPath;
	const char* p2sPath;

	SDL_Texture* p2sTexture;
	int v_start = 0;

	// buttons
	List<GuiButton*> listButtons;
	const char* buttons[4] = { "Play", "Continue", "Options", "Credits" };
	/*
	GuiButton* button_play;
	GuiButton* button_continue;
	GuiButton* button_options;
	GuiButton* button_credits;
	*/
};

#endif // __INTROSCENE_H__