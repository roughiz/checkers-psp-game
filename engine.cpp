#include <pspgu.h>
#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <psppower.h>
#include <png.h>
#include <psprtc.h>

#include "engine.h"
#include "menu.h"
#include "about.h"
#include "checkers.h"
#include "callbacks.h"
#include "graphics.h"

Engine::Engine() {
	SetupCallbacks();
	initGraphics();
}

/*
Engine::~Engine() {
	delete menu;
	delete about;
	delete checkers;
}
*/

void Engine::Start() {
	isRunning = true;
	gamestate = TITLE_START;
	menuGamestate = MENU;
	sprintf(fpsDisplay, "FPS: ??");
	sceRtcGetCurrentTick(&fpsTickLast);
	tickResolution = sceRtcGetTickResolution();
	menu = 0;
	Engine::Run();
}

void Engine::Run() {
	while (isRunning) {
		sceCtrlReadBufferPositive(&pad, 1); //update user input
		switch(gamestate) {
			case TITLE_START:
				gamestate = TITLE;
				break;
			
			case TITLE:
				fillScreenColor(BLACK);
				printTextCenter("Hello Ilyes: PSP Checker JEU", WHITE);
				gamestate = TITLE_END;
				break;
				
			case TITLE_END:
				fillScreenColor(BLACK);
				gamestate = MENU_START;
				break;
			
			case MENU_START:
				menu = new Menu(MENU);
				menu->AddItem("Start Game", CHECKERS_START);
				menu->AddItem("Options", OPTIONS_START);
				menu->AddItem("About", ABOUT_START);
				gamestate = MENU;
				break;
			
			case MENU:
				fillScreenColor(BLACK);
				menuGamestate = menu->Run(pad);
				break;
				
			case MENU_END:
				delete menu;
				gamestate = menuGamestate;
				menuGamestate = MENU;
				break;
				
			case OPTIONS_START:
				gamestate = OPTIONS;
				break;
				
			case OPTIONS:
				fillScreenColor(BLACK);
				printTextCenter("options", WHITE);
				printTextCenter("Press CIRCLE to return to the menu screen", WHITE, (SCREEN_HEIGHT - 10));
				
				//need options class
				if (pad.Buttons & PSP_CTRL_CIRCLE) {
					gamestate = OPTIONS_END;
				}
				break;
				
			case OPTIONS_END:
				gamestate = MENU_START;
				break;
				
			case ABOUT_START:
				about = new About();
				about->Start();
				gamestate = ABOUT;
				break;
			
			case ABOUT:
				if (!about->Run(pad)) {
					gamestate = ABOUT_END;	
				}
				break;
				
			case ABOUT_END:
				delete about;
				gamestate = MENU_START;
				break;
			
			case CHECKERS_START:
				checkers = new Checkers();
				gamestate = CHECKERS;
				break;
				
			case CHECKERS:
				if (!checkers->Run(pad)) {
					gamestate = CHECKERS_END;
				}
				break;
				
			case CHECKERS_END:
				delete checkers;
				fillScreenColor(BLACK);
				gamestate = TITLE_START;
				break;
			
			default:
				fillScreenColor(BLACK);
				printTextCenter("gamestate default", WHITE);
				break;	
		}
		
		if (gamestate == MENU) {
			if (menuGamestate != gamestate) {
				gamestate = MENU_END;
			}
		}
		
		DrawScreen();
	}
}

void Engine::DrawScreen() {
	FPS();
	flipScreen();
	sceDisplayWaitVblankStart();
}

int Engine::GetFPS() {
	return fps;	
}

void Engine::FPS() {
	
	fps++;
	sceRtcGetCurrentTick( &fpsTickNow );
	
	if( ((fpsTickNow - fpsTickLast)/((float)tickResolution) ) >= 1.0f ) {
		fpsTickLast = fpsTickNow;
		//sprintf( fpsDisplay, "FPS: %d", fps );
		fps = 0;
	}
	//printTextScreen(0, 0, fpsDisplay, WHITE);
}
