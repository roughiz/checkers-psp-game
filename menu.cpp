#include <pspkernel.h>
#include <pspctrl.h>
#include <psprtc.h>

#include "menu.h"
#include "menuitem.h"
#include "graphics.h"
#include "engine.h"


Menu::Menu(gstate dgstate) {
	selected = 0;
	gamestate = dgstate;
	menuItems = 0;
	tickResolution = sceRtcGetTickResolution();
	sceRtcGetCurrentTick( &lastMenuMove );
	sceRtcGetCurrentTick( &startMenuTime );
}

Menu::~Menu() {
	delete menuData;
}

void Menu::AddItem(char *menuText, gstate gs) {
	MenuItem data;
	data.text = menuText;
	data.gs = gs;
	menuData[menuItems] = data;
	menuItems++;
}

gstate Menu::Run(SceCtrlData &pad) {
	
	sceRtcGetCurrentTick( &thisMenuMove );
	if( ((thisMenuMove - lastMenuMove)/((float)tickResolution)) >= MENU_DELAY) {
		if (pad.Buttons & PSP_CTRL_DOWN) {
			if (selected < (menuItems - 1)) {
				selected++;
				lastMenuMove = thisMenuMove;
			}
		}
		if (pad.Buttons & PSP_CTRL_UP) {
			if (selected > 0) {
				selected--;
				lastMenuMove = thisMenuMove;
			}
		}
	}
	
	if( ((thisMenuMove - startMenuTime)/((float)tickResolution)) >= MENU_START_DELAY) {
		if (pad.Buttons & PSP_CTRL_CROSS) {
			return menuData[selected].gs;
		}
	}
	
	for (int i=0; i<menuItems; i++) {
		if (selected == i) {
			printTextCenter(menuData[i].text, RED, (MENU_START_Y + (i*15)));
		}
		else {
			printTextCenter(menuData[i].text, WHITE, (MENU_START_Y + (i*15)));
		}
	}
	
	printTextCenter("Use CROSS to select a menu item", WHITE, (SCREEN_HEIGHT - 10));
	
	return gamestate;
}
