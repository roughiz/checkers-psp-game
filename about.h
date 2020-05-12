#ifndef __ABOUT_H_
#define __ABOUT_H_
#include <pspctrl.h>

const int MESSAGE_SPACE = 13;

class About {
private:
	bool scroll;
	bool doExit;
	int printLoc;
	int aboutMessageTotal;
	int exitState;
	char *aboutMessage[100];
	
	bool DoExitScene();

public:
	About();
	virtual ~About();
	void Start();
	bool Run(SceCtrlData &pad);
};

#endif /*__ABOUT_H_*/
