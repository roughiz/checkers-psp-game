#include "about.h"
#include "graphics.h"

About::About() {
	aboutMessageTotal = 13;
	aboutMessage[0] = "PSPCheckers Version 0.1";
	aboutMessage[1] = " ";
	aboutMessage[2] = "Coded by Trevis <theredcheese@gmail.com>";
	aboutMessage[3] = "Website: http://haxit.org/";
	aboutMessage[4] = " ";
	aboutMessage[5] = "This is my first real psp application/game.";
	aboutMessage[6] = "It was written in C++ on linux";
	aboutMessage[7] = "using Eclipse IDE and psplink.  ";
	aboutMessage[8] = " ";
	aboutMessage[9] = "Thanks to:";
	aboutMessage[10] = "Jessie: Moral support, and dealing with me :D";
	aboutMessage[11] = "Charles: Providing the idea";
	aboutMessage[12] = "The Internet: Helping me out with learning C++";
	
	printLoc = SCREEN_HEIGHT - (aboutMessageTotal * MESSAGE_SPACE);
	
	exitState = 0;
	
	scroll = true;
	doExit = false;
}

About::~About() {
	delete aboutMessage;
}

void About::Start() {
	
}

bool About::DoExitScene() {
	if (exitState < 100) { //fixy :/
		fillScreenRect(0x00FFFFFF, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		exitState++;
		return true;
	}
	else {
		return false;
	}
}

bool About::Run(SceCtrlData &pad) {
	fillScreenColor(BLACK);
	for(int i=0; i<aboutMessageTotal; i++) {
			if (((i*MESSAGE_SPACE) + printLoc) > SCREEN_HEIGHT) {
				printTextCenter(aboutMessage[i], WHITE, ((i*MESSAGE_SPACE) + printLoc - SCREEN_HEIGHT));
			}
			else {
				printTextCenter(aboutMessage[i], WHITE, ((i*MESSAGE_SPACE) + printLoc));	
			}
	}
	
	if (scroll) {
		if (printLoc <= 0) {
			printLoc = SCREEN_HEIGHT;
		}
		else {
			printLoc--;
		}
		scroll = false;
	}
	else {
		scroll = true;	
	}
		
	if (pad.Buttons & PSP_CTRL_CIRCLE) {
		doExit = true;	
	}
	
	if (pad.Buttons & PSP_CTRL_SQUARE) {
		doExit = true;	
	}
	
	if (pad.Buttons & PSP_CTRL_TRIANGLE) {
		doExit = true;	
	}
	
	if (doExit) {
		return DoExitScene();
	}
		
	return true;
}

