#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "App.h"
#include "GuiManager.h"

#include "GuiButton.h"
#include "GuiCheckBox.h"
#include "GuiSliderBar.h"

#include "Window.h"
#include "Render.h"
#include "Audio.h"
#include "Module.h"

#include "Point.h"
#include "SString.h"

#include "SDL/include/SDL.h"
#include "Log.h"



struct Settings
{
public:

	Settings* CreateSettings(Module* mod)
	{
		Settings* set = this;

		//settingsTexture = app->tex->Load(settingsPath);

		// settings buttons
		settings = false;
		open = false;

		// close
		GUI_id++;
		GuiButton* button = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, GUI_id, "x", { 137, 56, 26, 28 }, 10, mod, ButtonType::SMALL);
		button->state = GuiControlState::NONE;
		listSettingsButtons.Add(button);

		// music
		GUI_id++;
		GuiSliderBar* sliderBar = (GuiSliderBar*)app->guiManager->CreateGuiControl(GuiControlType::SLIDERBAR, GUI_id, "music", { 250, 170, 60, 10 }, 10, mod, ButtonType::NONE, { 310, 165, 14, 16});
		sliderBar->state = GuiControlState::NONE;
		music = sliderBar;
		listSliderBars.Add(sliderBar);

		// fx
		GUI_id++;
		sliderBar = (GuiSliderBar*)app->guiManager->CreateGuiControl(GuiControlType::SLIDERBAR, GUI_id, "fx", { 250, 210, 60, 10 }, 10, mod, ButtonType::NONE, { 310, 205, 14, 16 });
		sliderBar->state = GuiControlState::NONE;
		fx = sliderBar;
		listSliderBars.Add(sliderBar);


		// fullscreen	
		GUI_id++;
		GuiCheckBox* checkbox = (GuiCheckBox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, GUI_id, "", { 330, 240, 30, 30 }, 10, mod);
		checkbox->state = GuiControlState::NONE;
		listCheckbox.Add(checkbox);

		// vsync
		GUI_id++;
		checkbox = (GuiCheckBox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, GUI_id, "", { 330, 280, 30, 30 }, 10, mod);
		checkbox->state = GuiControlState::NONE;
		listCheckbox.Add(checkbox);

		return set;
	}

	bool OpenSettings()
	{
		SDL_Rect rect = { 0, 0, 226, 261 };

		app->render->DrawRectangle({ 150, 70, 226, 261 }, 206, 167, 240 , 230, true);
		app->render->DrawTexture(settingsTexture, 150, 70, &rect);

		int x = 170; int y = 130; int offset = 40;
		app->render->TextDraw("Music:", x, y + offset, 12);
		app->render->TextDraw("Fx:", x, y + offset * 2, 12);
		app->render->TextDraw("Fullscreen:", x, y + offset * 3, 12);
		app->render->TextDraw("Vsync:", x, y + offset * 4, 12);

		if (!open)
		{
			for (ListItem<GuiButton*>* i = listSettingsButtons.start; i != nullptr; i = i->next)
			{
				i->data->state = GuiControlState::NORMAL;
			}


			for (ListItem<GuiCheckBox*>* i = listCheckbox.start; i != nullptr; i = i->next)
			{
				if (i->data->id == 4 && app->win->changeScreen)
				{
					i->data->state = GuiControlState::SELECTED;
				}
				else
				{
					i->data->state = GuiControlState::NORMAL;
				}
			}

			for (ListItem<GuiSliderBar*>* i = listSliderBars.start; i != nullptr; i = i->next)
			{
				i->data->state = GuiControlState::NORMAL;
			}

			music->sliderBounds.x = music->bounds.x + app->audio->volumeM * 60 / SDL_MIX_MAXVOLUME;
			music->volume100 = app->audio->volumeM;

			fx->sliderBounds.x = fx->bounds.x + app->audio->volumeF * 60 / SDL_MIX_MAXVOLUME;
			fx->volume100 = app->audio->volumeF;


			open = true;
		}

		if (app->audio->volumeM != music->volume100)app->audio->ChangeMusicVolume(music->volume100);
		if (app->audio->volumeF != fx->volume100)app->audio->ChangeFxVolume(fx->volume100);
		return true;
	}

	bool CloseSettings()
	{
		settings = false;
		open = false;
		for (ListItem<GuiButton*>* i = listSettingsButtons.start; i != nullptr; i = i->next)
		{
			i->data->state = GuiControlState::NONE;
		}

		for (ListItem<GuiCheckBox*>* i = listCheckbox.start; i != nullptr; i = i->next)
		{
			i->data->state = GuiControlState::NONE;
		}

		for (ListItem<GuiSliderBar*>* i = listSliderBars.start; i != nullptr; i = i->next)
		{
			i->data->state = GuiControlState::NONE;
		}

		return true;
	}

	bool CleanUp()
	{
		//app->tex->UnLoad(settingsTexture);
		listSettingsButtons.Clear();
		listCheckbox.Clear();
		listSliderBars.Clear();

		return true;
	}

public:

	// buttons
	int GUI_id = 0;
	List<GuiButton*> listSettingsButtons;
	List<GuiSliderBar*> listSliderBars;
	List<GuiCheckBox*> listCheckbox;

	GuiSliderBar* music, *fx;

	SDL_Texture* settingsTexture;
	const char* settingsPath;
	bool settings;
	bool open;
};


struct Pause
{
public:

	Pause* CreatePause(Module* mod)
	{
		Pause* pause = this;

		//PauseTexture = app->tex->Load(PausePath);

		// Pause buttons
		pause = false;
		open = false;

		// close
		GUI_id++;
		GuiButton* button = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, GUI_id, "x", { 137, 56, 26, 28 }, 10, mod, ButtonType::SMALL);
		button->state = GuiControlState::NONE;
		listPauseButtons.Add(button);

		//// resume
		//GUI_id++;
		//button = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, GUI_id, "Resume", { 250, 170, 60, 10 }, 10, mod, ButtonType::LONG);
		//button->state = GuiControlState::NONE;
		//listPauseButtons.Add(button);

		//// return to title
		//GUI_id++;
		//button = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, GUI_id, "Return to Title", { 250, 210, 60, 10 }, 9, mod, ButtonType::LONG);
		//button->state = GuiControlState::NONE;
		//listPauseButtons.Add(button);
	
		//// settings
		//GUI_id++;
		//button = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, GUI_id, "Settings", { 250, 240, 60, 10 }, 10, mod, ButtonType::LONG);
		//button->state = GuiControlState::NONE;
		//listPauseButtons.Add(button);

		//// exit
		//GUI_id++;
		//button = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, GUI_id, "Exit", { 250, 210, 60, 10 }, 9, mod, ButtonType::LONG);
		//button->state = GuiControlState::NONE;
		//listPauseButtons.Add(button);

		// buttons
		for (int i = 0; buttons[i] != "\n"; i++)
		{
			button = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, i + 6, buttons[i], { 180, 170 + 33 * i, 90, 27 }, 10, mod, ButtonType::LONG);
			button->state = GuiControlState::NONE;
			listPauseButtons.Add(button);
		}

		return pause;
	}

	bool OpenPause()
	{
		SDL_Rect rect = { 0, 0, 226, 261 };

		app->render->DrawRectangle({ 150, 70, 226, 261 }, 206, 167, 240, 230, true);
		app->render->DrawTexture(PauseTexture, 150, 70, &rect);

		if (!open)
		{
			for (ListItem<GuiButton*>* i = listPauseButtons.start; i != nullptr; i = i->next)
			{
				i->data->state = GuiControlState::NORMAL;
			}

			open = true;
		}

		return true;
	}

	bool ClosePause()
	{
		pause = false;
		open = false;
		for (ListItem<GuiButton*>* i = listPauseButtons.start; i != nullptr; i = i->next)
		{
			i->data->state = GuiControlState::NONE;
		}

		return true;
	}

	bool CleanUp()
	{
		//app->tex->UnLoad(PauseTexture);
		listPauseButtons.Clear();
		
		return true;
	}

public:

	// buttons
	int GUI_id = 0;
	List<GuiButton*> listPauseButtons;
	const char* buttons[5] = { "Resume", "Return to Title", "Settings", "Exit", "\n" };

	SDL_Texture* PauseTexture;
	const char* PausePath;
	bool pause;
	bool open;
};

#endif // __SETTINGS_H__