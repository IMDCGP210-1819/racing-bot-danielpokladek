#include <map>
#include <stdio.h>
#include <iostream>

#include "AI.h"
#include "Blackboard.h"

#include "Sequence.h"
#include "Selector.h"
#include "Succeder.h"

#include "TreeNodes.h"

/* VARIABLES USED BY AI */
const float AI::MAX_UNSTUCK_ANGLE = 15.0 / 180.0*PI;
const float AI::UNSTUCK_TIME_LIMIT = 2.0;
const float AI::MAX_UNSTUCK_SPEED = 5.0;
const float AI::MIN_UNSTUCK_DIST = 3.0;
const float AI::SHIFT = 0.9;
const float AI::SHIFT_MARGIN = 4.0;
const float AI::FULL_ACCEL_MARGIN = 7.0;
const float AI::G = 9.81;
const float AI::ABS_SLIP = 0.9;
const float AI::ABS_MINSPEED = 3.0;
const float AI::TCL_SLIP = 0.9;
const float AI::TCL_MINSPEED = 3.0;
const float AI::LOOKAHEAD_CONST = 17.0;
const float AI::LOOKAHEAD_FACTOR = 0.33;
const float AI::WIDTHDIV = 4.0;


/* BEHAVIOR TREE COMPONENTS */
// ---- BEHAVIOR TREE
Sequence	*root;

// ---- Drive Sequences
Sequence	*driveSequence;

// ---- Control Acceleration
Succeder		*driveSucceder;
Selector		*driveSelector;
AccelerateNode	*accelNode;
BrakeNode		*brakeNode;

// ---- Control Steering
Succeder		*steerSucceder;
Selector		*steerSelector;
SteeringClass	*steerNode;
StuckNode		*stuckNode;

// ---- Control Gears
Succeder		*gearSucceder;
Selector		*gearSelector;
ShiftUpNode		*shiftUpNode;
ShiftDownNode	*shiftDownNode;

AI::AI()
{
	
}

AI::~AI()
{

}

/* Initialize the behavior tree, and add all the nodes into the root. */
void AI::initTree()
{
	std::cout << "AI -> Initializing BehaviorTree.\n";

	// ---- TREE ROOT
	root = new Sequence();
	driveSequence	= new Sequence();


	// ---- SPEED BRANCH
	driveSelector	= new Selector();
	accelNode		= new AccelerateNode(this);
	brakeNode		= new BrakeNode(this);

	driveSelector->addChild(brakeNode);
	driveSelector->addChild(accelNode);

	driveSucceder	= new Succeder(driveSelector);


	// ---- TURNING BRANCH
	steerSelector	= new Selector();
	steerNode		= new SteeringClass(this);
	stuckNode		= new StuckNode(this);

	steerSelector->addChild(stuckNode);
	steerSelector->addChild(steerNode);

	steerSucceder	= new Succeder(steerSelector);


	// ---- GEARS BRANCH
	gearSelector	= new Selector();
	shiftUpNode		= new ShiftUpNode(this);
	shiftDownNode	= new ShiftDownNode(this);

	gearSelector->addChild(shiftUpNode);
	gearSelector->addChild(shiftDownNode);

	gearSucceder	= new Succeder(gearSelector);


	// ---- ADD BRANCHES TO DRIVE SEQUENCE
	driveSequence->addChild(driveSucceder);
	driveSequence->addChild(steerSucceder);
	driveSequence->addChild(gearSucceder);
	

	// ---- ADD DRIVE SEQUENCE TO ROOT
	root->addChild(driveSequence);

	std::cout << "AI -> BehaviorTree initialized.\n";
}

/* Initialize the blackboard, and it's values. */
void AI::initBlackboard()
{
	std::cout << "BehaviorTree -> Initializing blackboard.\n";

	/* Initialize all the variables that will be used by the car, such as the acceleration amount,
		brakes amount, and the current gear the car should be in. */

	blackboard.insert(std::pair<std::string, BlackboardBaseType*>
		(stuckKey, new BlackboardBoolType(false)));
	blackboard.insert(std::pair<std::string, BlackboardBaseType*>
		(gearKey, new BlackboardIntType(1)));
	blackboard.insert(std::pair<std::string, BlackboardBaseType*>
		(accelKey, new BlackboardFloatType(0.0f)));
	blackboard.insert(std::pair<std::string, BlackboardBaseType*>
		(brakeKey, new BlackboardFloatType(0.0f)));
	blackboard.insert(std::pair<std::string, BlackboardBaseType*>
		(steerKey, new BlackboardFloatType(0.0)));


	std::cout << "BehaviorTree -> Blackboard initialized.\n";
}

/* Called at the start of a new race */
void AI::newRace(tCarElt *car)
{
	MAX_UNSTUCK_COUNT = int(UNSTUCK_TIME_LIMIT / RCM_MAX_DT_ROBOTS);
	stuck = 0;

	initBlackboard();
	initTree();

	CARMASS = GfParmGetNum(car->_carHandle, SECT_CAR, PRM_MASS, NULL, 1000.0);
	
	initCA();
	initCW();

	initTCLFilter();
}

/* Update function used to drive the car */
void AI::drive(tCarElt* car)
{
	// Clear the car's memory.
	memset((void *)&car->ctrl, 0, sizeof(tCarCtrl));

	// Calculate car's mass, with the current fuel load.
	mass = CARMASS + car->_fuel;

	// Update the tree
	root->tick();
	
	/* Get the variables from the blackboard to drive the car. The variables need be read this way,
		because the other way it caused it to crash. This was because the pointers were intialized with
		different memory adresses and the game was crashing because the entry simply didn't exist. */

	BlackboardIntType *gearEntry = (BlackboardIntType*)blackboard.at(gearKey);
	currGear = gearEntry->GetValue();
	
	BlackboardFloatType *accelEntry = (BlackboardFloatType*)blackboard.at(accelKey);
	accelValue = accelEntry->GetValue();

	BlackboardFloatType *brakeEntry = (BlackboardFloatType*)blackboard.at(brakeKey);
	brakeValue = brakeEntry->GetValue();

	BlackboardFloatType *steerEntry = (BlackboardFloatType*)blackboard.at(steerKey);
	steerAngle = steerEntry->GetValue();


	// Use the blackboard values to drive the car.
	car->ctrl.steer = steerAngle / car->_steerLock;

	car->ctrl.brakeCmd = filterABS(brakeValue);

	if (car->ctrl.brakeCmd == 0.0)
		car->ctrl.accelCmd = filterTCL(accelValue);
	else
		car->ctrl.accelCmd = 0.0;

	car->ctrl.gear = currGear;

	std::cout << "Accel Value: " << car->ctrl.accelCmd << " | Brake Value: " << car->ctrl.brakeCmd << std::endl;
}

#pragma region UTILITY FUNCTIONS
/* Hold car on the track */
float AI::filterTrack(float accel)
{
	tTrackSeg *segment = carReference->_trkPos.seg;

	if (carReference->_speed_x < MAX_UNSTUCK_SPEED)
		return accel;

	if (segment->type = TR_STR)
	{
		float tm = fabs(carReference->_trkPos.toMiddle);
		float w = segment->width / WIDTHDIV;

		if (tm > w)
			return 0.0;
		else
			return accel;
	}
	else
	{
		float sign = (segment->type == TR_RGT) ? -1 : 1;
		if (carReference->_trkPos.toMiddle*sign > 0.0)
		{
			return accel;
		}
		else
		{
			float tm = fabs(carReference->_trkPos.toMiddle);
			float w = segment->width / WIDTHDIV;
			if (tm > w)
				return 0.0;
			else
				return accel;
		}
	}
}

/* Get the point on the track, to which the car will drive */
v2d AI::getTargetPoint()
{
	tTrackSeg *seg = carReference->_trkPos.seg;
	float lookAhead = LOOKAHEAD_CONST + carReference->_speed_x*LOOKAHEAD_FACTOR;

	float length = getDistToSegEnd();

	while (length < lookAhead)
	{
		seg = seg->next;
		length += seg->length;
	}

	length = lookAhead - length + seg->length;
	v2d s;

	s.x = (seg->vertex[TR_SL].x + seg->vertex[TR_SR].x) / 2.0;
	s.y = (seg->vertex[TR_SL].y + seg->vertex[TR_SR].y) / 2.0;

	if (seg->type == TR_STR)
	{
		v2d d;
		d.x = (seg->vertex[TR_EL].x - seg->vertex[TR_SL].x) / seg->length;
		d.y = (seg->vertex[TR_EL].y - seg->vertex[TR_SL].y) / seg->length;
		return s + d * length;
	}
	else
	{
		v2d c;
		c.x = seg->center.x;
		c.y = seg->center.y;
		float arc = length / seg->radius;
		float arcsign = (seg->type == TR_RGT) ? -1 : 1;
		arc = arc * arcsign;
		return s.rotate(c, arc);
	}
}

/* Check if the car is stuck */
bool AI::isStuck()
{
	BlackboardBoolType *stuckEntry = (BlackboardBoolType*)blackboard.at(stuckKey);

	if (fabs(steerAngle) > MAX_UNSTUCK_ANGLE &&
		carReference->_speed_x < MAX_UNSTUCK_SPEED &&
		fabs(carReference->_trkPos.toMiddle) > MIN_UNSTUCK_DIST)
	{
		if (stuck > MAX_UNSTUCK_COUNT && carReference->_trkPos.toMiddle*steerAngle < 0.0)
		{
			stuckEntry->SetValue(true);
			return true;
		}
		else
		{
			stuck++;
			stuckEntry->SetValue(false);
			return false;
		}
	}
	else
	{
		stuck = 0;
		stuckEntry->SetValue(false);
		return false;
	}
}

/* Get the max allowed speed on the segment */
float AI::getAllowedSpeed(tTrackSeg *segment)
{
	if (segment->type == TR_STR)
	{
		return FLT_MAX;
	}
	else
	{
		/*float mu = segment->surface->kFriction;
		return sqrt((mu*G*segment->radius) / (1.0 - MIN(1.0, segment->radius*CA*mu / mass)));*/

		float arc = 0.0;
		tTrackSeg *seg = segment;

		while (seg->type == segment->type && arc < PI / 2.0)
		{
			arc += seg->arc;
			seg = seg->next;
		}

		arc /= PI / 2.0;
		float mu = segment->surface->kFriction;
		float r = (segment->radius + segment->width / 2.0) / sqrt(arc);
		return sqrt((mu*G*r) / (1.0 - MIN(1.0, r*CA*mu / mass)));
	}
}

/* Get the distance to the end of the segment */
float AI::getDistToSegEnd()
{
	if (carReference->_trkPos.seg->type == TR_STR)
		return carReference->_trkPos.seg->length - carReference->_trkPos.toStart;
	else
		return (carReference->_trkPos.seg->arc - carReference->_trkPos.toStart)*carReference->_trkPos.seg->radius;
}

/* Get the aerodynamic downforce coefficient CA */
void AI::initCA()
{
	char *WheelSect[4] = { SECT_FRNTRGTWHEEL, SECT_FRNTLFTWHEEL,
							SECT_REARRGTWHEEL, SECT_REARLFTWHEEL };

	float rearWingArea = GfParmGetNum(carReference->_carHandle, SECT_REARWING,
										PRM_WINGAREA, (char*)NULL, 0.0);

	float rearWingAngle = GfParmGetNum(carReference->_carHandle, SECT_REARWING,
										PRM_WINGANGLE, (char*) NULL, 0.0);

	float wingCA = 1.23 * rearWingArea*sin(rearWingAngle);

	float cl =	GfParmGetNum(carReference->_carHandle, SECT_AERODYNAMICS,
								PRM_FCL, (char*) NULL, 0.0) +
				GfParmGetNum(carReference->_carHandle, SECT_AERODYNAMICS,
								PRM_RCL, (char*) NULL, 0.0);

	float h = 0.0;
	int i;

	for (i = 0; i < 4; i++)
		h += GfParmGetNum(carReference->_carHandle, WheelSect[i],
							PRM_RIDEHEIGHT, (char*) NULL, 0.20);

	h *= 1.5;
	h = h * h;
	h = 2.0 * exp(-3.0*h);

	CA = h * cl + 4.0*wingCA;
}

/* Get aerodynamic drag coefficient CW */
void AI::initCW()
{
	float cx = GfParmGetNum(carReference->_carHandle, SECT_AERODYNAMICS,
								PRM_CX, (char*) NULL, 0.0);
	float frontArea = GfParmGetNum(carReference->_carHandle, SECT_AERODYNAMICS,
										PRM_FRNTAREA, (char*) NULL, 0.0);

	CW = 0.645*cx*frontArea;
}

/* Anti-Lock Brake System - avoid the car's tyres from locking up. */
float AI::filterABS(float brake)
{
	if (carReference->_speed_x < ABS_MINSPEED)
		return brake;

	int i;
	float slip = 0.0;
	for (i = 0; i < 4; i++)
	{
		slip += carReference->_wheelSpinVel(i) * carReference->_wheelRadius(i) / carReference->_speed_x;
	}

	slip = slip / 4.0;

	if (slip < ABS_SLIP)
		brake = brake * slip;

	return brake;
}

/* Traction Control System - avoid the car's tyres from spinning due to too much acceleration */
float AI::filterTCL(float accel)
{
	if (carReference->_speed_x < TCL_MINSPEED)
		return accel;

	float slip = carReference->_speed_x / (this->*GET_DRIVEN_WHEEL_SPEED)();

	if (slip < TCL_SLIP)
	{
		accel = 0.0;
	}

	return accel;
}

/* Traction Control (TCL) setup */
void AI::initTCLFilter()
{
	char const *traintype = GfParmGetStr(carReference->_carHandle, SECT_DRIVETRAIN, PRM_TYPE, VAL_TRANS_RWD);

	if (strcmp(traintype, VAL_TRANS_RWD) == 0)
	{
		GET_DRIVEN_WHEEL_SPEED = &AI::filterTCL_RWD;
	}
	else if (strcmp(traintype, VAL_TRANS_FWD) == 0)
	{
		GET_DRIVEN_WHEEL_SPEED = &AI::filterTCL_FWD;
	}
	else if (strcmp(traintype, VAL_TRANS_4WD) == 0)
	{
		GET_DRIVEN_WHEEL_SPEED = &AI::filterTCL_4WD;
	}
}

/* TCL filter plugin for rear wheel driven cars */
float AI::filterTCL_RWD()
{
	return (carReference->_wheelSpinVel(REAR_RGT) + carReference->_wheelSpinVel(REAR_LFT)) *
		carReference->_wheelRadius(REAR_LFT) / 2.0;
}

/* TCL filter plugin for front wheel driven cars */
float AI::filterTCL_FWD()
{
	return (carReference->_wheelSpinVel(FRNT_RGT) + carReference->_wheelSpinVel(FRNT_LFT)) *
		carReference->_wheelRadius(FRNT_LFT) / 2.0;
}

/* TCL filter plugin for all wheel driven cars */
float AI::filterTCL_4WD()
{
	return (carReference->_wheelSpinVel(REAR_RGT) + carReference->_wheelSpinVel(REAR_LFT)) *
		carReference->_wheelRadius(REAR_LFT) / 4.0 +
		(carReference->_wheelSpinVel(FRNT_RGT) + carReference->_wheelSpinVel(FRNT_LFT)) *
		carReference->_wheelRadius(FRNT_LFT) / 4.0;
}
#pragma endregion