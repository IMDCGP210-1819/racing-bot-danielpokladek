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
	void drive(tCarElt* car);

protected:
	std::map<std::string, BlackboardBaseType*> blackboard;

	const std::string gear = "currGear";

	float brakeAmnt = 0.0f;
	float accelAmnt = 0.0f;
	int currGear	= 0;
};

