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
const float AI::ABS_SLIP = 0.9;
const float AI::ABS_MINSPEED = 3.0;
const float AI::TCL_SLIP = 0.9;
const float AI::TCL_MINSPEED = 3.0;


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
ToRaceLineNode	*racingLineNode;
KeepDrivingNode	*keepDrivingNode;
SteeringClass	*steerNode;

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
	keepDrivingNode = new KeepDrivingNode(this);
	racingLineNode	= new ToRaceLineNode(this);
	steerNode		= new SteeringClass(this);

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

void AI::initBlackboard()
{
	std::cout << "BehaviorTree -> Initializing blackboard.\n";

	/* Initialize all the variables that will be used by the car, such as the acceleration amount,
		brakes amount, and the current gear the car should be in. */

	blackboard.insert(std::pair<std::string, BlackboardBaseType*>
		(stuckBool, new BlackboardBoolType(false)));
	blackboard.insert(std::pair<std::string, BlackboardBaseType*>
		(gear, new BlackboardIntType(1)));
	blackboard.insert(std::pair<std::string, BlackboardBaseType*>
		(accel, new BlackboardFloatType(0.0f)));
	blackboard.insert(std::pair<std::string, BlackboardBaseType*>
		(brake, new BlackboardFloatType(0.0f)));
	blackboard.insert(std::pair<std::string, BlackboardBaseType*>
		(steerKey, new BlackboardFloatType(0.0)));


	std::cout << "BehaviorTree -> Blackboard initialized.\n";
}

void AI::drive(tCarElt* car)
{
	mass = CARMASS + car->_fuel;

	// Update the tree
	root->tick();

	// Clear the car's memory.
	//memset((void *)&car->ctrl, 0, sizeof(tCarCtrl));
	
	/* Get the variables from the blackboard to drive the car. The variables need be read this way,
		because the other way it caused it to crash. This was because the pointers were intialized with
		different memory adresses and the game was crashing because the entry simply didn't exist. */

	BlackboardIntType *gearEntry = (BlackboardIntType*)blackboard.at(gear);
	currGear = gearEntry->GetValue();
	
	BlackboardFloatType *accelEntry = (BlackboardFloatType*)blackboard.at(accel);
	accelValue = accelEntry->GetValue();

	BlackboardFloatType *brakeEntry = (BlackboardFloatType*)blackboard.at(brake);
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
}

#pragma region UTILITY FUNCTIONS
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

void AI::initCW()
{
	float cx = GfParmGetNum(carReference->_carHandle, SECT_AERODYNAMICS,
								PRM_CX, (char*) NULL, 0.0);
	float frontArea = GfParmGetNum(carReference->_carHandle, SECT_AERODYNAMICS,
										PRM_FRNTAREA, (char*) NULL, 0.0);

	CW = 0.645*cx*frontArea;
}

float AI::getAllowedSpeed(tTrackSeg *segment)
{
	if (segment->type == TR_STR)
	{
		return FLT_MAX;
	}
	else
	{
		float mu = segment->surface->kFriction;
		return sqrt((mu*G*segment->radius) / (1.0 - MIN(1.0, segment->radius*CA*mu / mass)));
	}
}

float AI::getDistToSegEnd()
{
	if (carReference->_trkPos.seg->type == TR_STR)
		return carReference->_trkPos.seg->length - carReference->_trkPos.toStart;
	else
		return (carReference->_trkPos.seg->arc - carReference->_trkPos.toStart)*carReference->_trkPos.seg->radius;
}

float AI::getAccel(tCarElt *car)
{
	float gear			= car->_gearRatio[car->_gear + car->_gearOffset];
	float allowedSpeed	= getAllowedSpeed(car->_trkPos.seg);
	float engineRedline = car->_enginerpmRedLine;

	if (allowedSpeed > car->_speed_x + FULL_ACCEL_MARGIN)
		return 1.0;
	else
		return allowedSpeed / car->_wheelRadius(REAR_RGT)*gear / engineRedline;
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