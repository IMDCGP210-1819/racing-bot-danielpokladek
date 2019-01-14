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

	/* ROOT OF BEHAVIOR TREE */
	root = new Sequence();

	/* DRIVE SEQUENCE */
	driveSequence	= new Sequence();

	// ---- ACCELERATION
	driveSelector	= new Selector();
	accelNode		= new AccelerateNode(this);
	brakeNode		= new BrakeNode(this);

	driveSelector->addChild(accelNode);
	driveSelector->addChild(brakeNode);

	driveSucceder	= new Succeder(driveSelector);

	// ---- TURNING
	steerSelector	= new Selector();
	keepDrivingNode = new KeepDrivingNode(this);
	racingLineNode	= new ToRaceLineNode(this);

	steerSelector->addChild(keepDrivingNode);

	steerSucceder	= new Succeder(steerSelector);


	// ---- ADD BRANCHES
	driveSequence->addChild(driveSucceder);
	driveSequence->addChild(steerSucceder);
	
	root->addChild(driveSequence);

	std::cout << "AI -> BehaviorTree initialized.\n";
}

void AI::initBlackboard()
{
	std::cout << "BehaviorTree -> Initializing blackboard.\n";

	/* Initialize all the variables that will be used by the car, such as the acceleration amount,
		brakes amount, and the current gear the car should be in. */

	
	blackboard.insert(std::pair<std::string, BlackboardBaseType*>
		(gear, new BlackboardIntType(1)));
	blackboard.insert(std::pair<std::string, BlackboardBaseType*>
		(accel, new BlackboardFloatType(0.0f)));
	blackboard.insert(std::pair<std::string, BlackboardBaseType*>
		(brake, new BlackboardFloatType(0.0f)));


	std::cout << "BehaviorTree -> Blackboard initialized.\n";
}

void AI::drive(tCarElt* car)
{
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

	/*Selector *root = new Selector();
	EdgeDistanceCheck *edgeCheck = new EdgeDistanceCheck();

	edgeCheck->AddChild(new AccelerationNode());
	root->AddChild(edgeCheck);

	root->AddChild(new Sequence());*/

	car->ctrl.brakeCmd = brakeAmnt;
	car->ctrl.accelCmd = accelAmnt;
	car->ctrl.gear = currGear;
}
