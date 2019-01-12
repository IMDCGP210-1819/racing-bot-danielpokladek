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

	const std::string accel = "accelAmnt";
	AI *ai;

protected:
	/* BLACKBOARD KEYS */
	const std::string gear = "currGear";
	const std::string brake = "brakeAmnt";

	const std::string initFinished = "initFinished";

	float brakeAmnt = 0.0f;
	float accelAmnt = 0.0f;
	int currGear	= 0;
};

