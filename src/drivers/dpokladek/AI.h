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
	
	/* UTILITY FUNCTIONS */
	void initCW();
	void initCA();
	
	float getAllowedSpeed(tTrackSeg *segment);
	float getDistToSegEnd();
	float getAccel(tCarElt * car);

	float filterABS(float brake);
	float filterTCL(float accel);
	float filterTCL_RWD();
	float filterTCL_FWD();
	float filterTCL_4WD();
	void initTCLFilter();

	float (AI::*GET_DRIVEN_WHEEL_SPEED) ();

	std::map<std::string, BlackboardBaseType*> blackboard;
	tCarElt* carReference;

	/* BLACKBOARD KEYS */
	const std::string gear			= "gearValue";
	const std::string brake			= "brakeValue";
	const std::string accel			= "accelValue";
	const std::string steerKey		= "steerAngle";
	const std::string car			= "carRef";
	const std::string stuckBool		= "stuckBool";

	AI *ai;

	const float SHIFT				= 0.9;
	const float SHIFT_MARGIN		= 4.0;
	const float G					= 9.81;
	const float FULL_ACCEL_MARGIN	= 1.0;

	static const float ABS_SLIP;
	static const float ABS_MINSPEED;
	static const float TCL_MINSPEED;
	static const float TCL_SLIP;

	float mass;
	float CARMASS;
	float CA;
	float CW;

protected:
	float steerAngle	= 0.0;
	float brakeValue	= 0.0;
	float accelValue	= 0.0;
	int currGear		= 0;
};

