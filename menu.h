#ifndef __MENU_H_
#define __MENU_H_

#include <pspkernel.h>
#include <pspctrl.h>
#include "gstate.h"
#include "menuitem.h"

const int MENU_START_Y = 150;
const float MENU_DELAY = 0.25f;
const float MENU_START_DELAY = 0.5f;

class Menu {
	private:
		int selected;
		int menuItems;
		u64 thisMenuMove;
		u64 lastMenuMove;
		u64 startMenuTime;
		u32 tickResolution;
		gstate gamestate;
		MenuItem menuData[10];

	public:
		Menu(gstate dgstate);
		virtual ~Menu();
		gstate Run(SceCtrlData &pad);
		void AddItem(char *menuText, gstate gs);
};

#endif /*__MENU_H_*/
