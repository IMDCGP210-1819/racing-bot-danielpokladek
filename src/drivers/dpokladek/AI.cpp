#include <map>
#include <stdio.h>
#include <iostream>

#include "AI.h"
#include "Blackboard.h"

#include "Sequence.h"
#include "Selector.h"
#include "Succeder.h"

#include "TreeNodes.h"

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
		(steerAngle, new BlackboardFloatType(0.0)));


	std::cout << "BehaviorTree -> Blackboard initialized.\n";
}

void AI::drive(tCarElt* car)
{
	// Update the tree
	root->tick();

	// Clear the car's memory.
	memset((void *)&car->ctrl, 0, sizeof(tCarCtrl));
	
	/* Get the variables from the blackboard to drive the car. The variables need be read this way,
		because the other way it caused it to crash. This was because the pointers were intialized with
		different memory adresses and the game was crashing because the entry simply didn't exist. */

	BlackboardIntType *gearEntry = (BlackboardIntType*)blackboard.at(gear);
	currGear = gearEntry->GetValue();
	
	BlackboardFloatType *accelEntry = (BlackboardFloatType*)blackboard.at(accel);
	accelAmnt = accelEntry->GetValue();

	BlackboardFloatType *brakeEntry = (BlackboardFloatType*)blackboard.at(brake);
	brakeAmnt = brakeEntry->GetValue();

	BlackboardFloatType *steerEntry = (BlackboardFloatType*)blackboard.at(steerAngle);
	angle = steerEntry->GetValue();


	// Use the blackboard values to drive the car.
	car->ctrl.steer = angle / car->_steerLock;

	car->ctrl.brakeCmd = brakeAmnt;
	if (car->ctrl.brakeCmd == 0.0)
		car->ctrl.accelCmd = accelAmnt;
	else
		car->ctrl.accelCmd = 0.0;

	car->ctrl.gear = 4;
}

#pragma region UTILITY FUNCTIONS
float AI::getAllowedSpeed(tTrackSeg *segment)
{
	if (segment->type == TR_STR)
	{
		return FLT_MAX;
	}
	else
	{
		float mu = segment->surface->kFriction;
		return sqrt(mu*G*segment->radius);
	}
}

float AI::getDistToSegEnd(tCarElt *car)
{
	if (car->_trkPos.seg->type == TR_STR)
		return car->_trkPos.seg->length - car->_trkPos.toStart;
	else
		return (car->_trkPos.seg->arc - car->_trkPos.toStart)*car->_trkPos.seg->radius;
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
#pragma endregion