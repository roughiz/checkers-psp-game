#include <pspkernel.h>

#include "engine.h"

PSP_MODULE_INFO("PSPCheckers", 0, 1, 1);

int main(void) {
	Engine gameEngine = Engine();
	gameEngine.Start();
}
