#pragma once
#include <map>
#include <string>

#include "linalg.h"
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
	void newRace(tCarElt* car);
	void drive(tCarElt* car);
	
	/* UTILITY FUNCTIONS */
	void initCW();
	void initCA();
	
	float getAllowedSpeed(tTrackSeg *segment);
	float getDistToSegEnd();

	bool isStuck();
	v2d getTargetPoint();
	float filterTrack(float accel);
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
	const std::string gearKey			= "gearValue";
	const std::string brakeKey			= "brakeValue";
	const std::string accelKey			= "accelValue";
	const std::string steerKey			= "steerAngle";
	const std::string stuckKey			= "stuckBool";

	int MAX_UNSTUCK_COUNT;

	static const float MAX_UNSTUCK_ANGLE;
	static const float UNSTUCK_TIME_LIMIT;
	static const float MAX_UNSTUCK_SPEED;
	static const float MIN_UNSTUCK_DIST;
	static const float SHIFT;
	static const float SHIFT_MARGIN;
	static const float FULL_ACCEL_MARGIN;
	static const float G;
	static const float ABS_SLIP;
	static const float ABS_MINSPEED;
	static const float TCL_MINSPEED;
	static const float TCL_SLIP;
	static const float LOOKAHEAD_CONST;
	static const float LOOKAHEAD_FACTOR;
	static const float WIDTHDIV;

	int stuck;

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

