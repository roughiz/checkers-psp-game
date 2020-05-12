#ifndef __ENGINE_H_
#define __ENGINE_H_

#include "gstate.h"
#include "menu.h"
#include "about.h"
#include "checkers.h"

class Engine {
	private:
		SceCtrlData pad;
		char fpsDisplay[100];
		Menu* menu;
		About* about;
		Checkers* checkers;
		int fps;
		u32 tickResolution;
		u64 fpsTickNow;
		u64 fpsTickLast;
		bool isRunning;
		gstate gamestate;
		gstate menuGamestate;
		void Run(void);
		void DrawScreen(void);
		void FPS(void);
		
	public:
		Engine(void);
		void Start(void);
		int GetFPS(void);
};

#endif /*__ENGINE_H_*/
