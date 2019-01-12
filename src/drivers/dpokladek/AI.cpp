#include <map>
#include <stdio.h>
#include <iostream>

#include "AI.h"
#include "Blackboard.h"

#include "Sequence.h"
#include "Selector.h"
#include "AccelerateNode.h"

//#include "Behavior.h"
//class AccelerationNode : public Behavior
//{
//	virtual Status update() override {
//		// access blackboard and set acceleration command value to x.
//	}
//};

Selector *root;
Selector *accelSel;
AccelerateNode *accelNode;

AI::AI()
{
	
}

AI::~AI()
{

}

void AI::initTree()
{
	std::cout << "AI -> Initializing BehaviorTree.\n";

	/* ROOT OF THE TREE */
	//Selector *root = new Selector();

	root = new Selector();

	// Create the accelerate sequence :
	//Sequence *accelSeq = new Sequence();
	accelSel = new Selector();
	accelNode = new AccelerateNode();
	accelSel->addChild(accelNode);

	root->addChild(accelSel);

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
	//root->tick();

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
