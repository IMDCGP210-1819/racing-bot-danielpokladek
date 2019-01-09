#include "AI.h"



void AI::drive(tCarElt* car)
{
	memset((void *)&car->ctrl, 0, sizeof(tCarCtrl));
	car->ctrl.brakeCmd = 0.0f;
	car->ctrl.accelCmd = 1.0f;
}
