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
	float getAllowedSpeed(tTrackSeg *segment);
	float getDistToSegEnd(tCarElt *car);
	float getAccel(tCarElt * car);

	std::map<std::string, BlackboardBaseType*> blackboard;
	tCarElt* carReference;

	/* BLACKBOARD KEYS */
	const std::string gear			= "gearValue";
	const std::string brake			= "brakeValue";
	const std::string accel			= "accelValue";
	const std::string steerAngle	= "steerAngle";
	const std::string car			= "carRef";
	const std::string stuckBool		= "stuckBool";

	AI *ai;

	const float SHIFT				= 0.9;
	const float SHIFT_MARGIN		= 4.0;
	const float G					= 9.81;
	const float FULL_ACCEL_MARGIN	= 1.0;

protected:
	float angle		= 0.0;
	float brakeAmnt = 0.0;
	float accelAmnt = 0.0;
	int currGear	= 0;
};

