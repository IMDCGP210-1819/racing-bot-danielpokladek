#pragma once
#include <map>
#include <string>

#include "car.h"
#include "robot.h"
#include "Blackboard.h"

class AI
{
public:
	AI();
	~AI();

	void initTree();
	void initBlackboard();
	void drive(tCarElt* car);

	std::map<std::string, BlackboardBaseType*> blackboard;
	tCarElt* carReference;

	/* BLACKBOARD KEYS */
	const std::string gear	= "gearValue";
	const std::string brake = "brakeValue";
	const std::string accel = "accelValue";
	const std::string car	= "carRef";
	AI *ai;

protected:
	const std::string initFinished = "initFinished";

	float brakeAmnt = 0.0f;
	float accelAmnt = 0.0f;
	int currGear	= 0;
};

