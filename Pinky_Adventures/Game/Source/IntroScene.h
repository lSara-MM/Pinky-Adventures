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
	bool OpenSettings();
	bool CloseSettings();

public:

	Player* player;
	bool loaded;

private:

	const char* musicIntro;

	SDL_Texture* bgTexture;
	const char* bgPath;

	// buttons
	List<GuiButton*> listButtons;
	const char* buttons[5] = { "Play", "Continue", "Settings", "Credits", "\n" };
	int bNum = 4;
	List<GuiButton*> listSettingsButtons;
	
	SDL_Texture* settingsTexture;
	const char* settingsPath;
	bool settings;
	bool open = false;
};

#endif // __INTROSCENE_H__