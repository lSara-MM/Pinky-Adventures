#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "App.h"
#include "GuiManager.h"

#include "GuiButton.h"
#include "GuiCheckBox.h"
#include "GuiSlider.h"

#include "Window.h"
#include "Render.h"
#include "Module.h"

#include "Point.h"
#include "SString.h"

#include "SDL/include/SDL.h"


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
		//listButtons.Add(button);
		listSettingsButtons.Add(button);

		// music
		GUI_id++;
		/*GuiButton* button = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, GUI_id, "x", { 137, 56, 26, 28 }, 10, mod, ButtonType::SMALL);
		button->state = GuiControlState::NONE;
		listButtons.Add(button);
		listSettingsButtons.Add(button);*/

		// sfx
		GUI_id++;
		/*GuiButton* button = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, GUI_id, "x", { 137, 56, 26, 28 }, 10, mod, ButtonType::SMALL);
		button->state = GuiControlState::NONE;
		listButtons.Add(button);
		listSettingsButtons.Add(button);*/


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
		app->render->DrawTexture(settingsTexture, 150, 70, &rect);

		int x = 170; int y = 130; int offset = 40;
		app->render->TextDraw("Music:", x, y + offset, 12);
		app->render->TextDraw("Sfx:", x, y + offset * 2, 12);
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
				if (i->data->id == 9 && app->win->changeScreen)
				{
					i->data->state = GuiControlState::SELECTED;
				}
				else
				{
					i->data->state = GuiControlState::NORMAL;
				}
			}

			open = true;
		}

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

		return true;
	}

	bool CleanUp()
	{
		//app->tex->UnLoad(settingsTexture);
		listSettingsButtons.Clear();
		listCheckbox.Clear();
	}

public:

	// buttons
	int GUI_id;
	List<GuiButton*> listSettingsButtons;

	List<GuiCheckBox*> listCheckbox;


	SDL_Texture* settingsTexture;
	const char* settingsPath;
	bool settings;
	bool open;

};

#endif // __SETTINGS_H__