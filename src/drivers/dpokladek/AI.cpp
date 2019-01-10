#include <map>
#include <stdio.h>
#include <iostream>

#include "AI.h"
#include "Blackboard.h"

//#include "Behavior.h"
//class AccelerationNode : public Behavior
//{
//	virtual Status update() override {
//		// access blackboard and set acceleration command value to x.
//	}
//};

AI::AI()
{
	std::cout << "Initializing the AI script\n";
}

AI::~AI()
{
}

void AI::initTree()
{
	std::cout << "Initializing the blackboard values\n";

	blackboard.insert(std::pair<std::string, BlackboardBaseType*>
		(gear, new BlackboardIntType(1)));
	blackboard.insert(std::pair<std::string, BlackboardBaseType*>
		("aceelAmnt", new BlackboardFloatType(0.0f)));
	blackboard.insert(std::pair<std::string, BlackboardBaseType*>
		("brakeAmnt", new BlackboardFloatType(1.0f)));

	std::cout << "Blackboard init finished\n";
}

void AI::drive(tCarElt* car)
{
	std::cout << "Drive the car!\n";

	memset((void *)&car->ctrl, 0, sizeof(tCarCtrl));
	
	BlackboardIntType *gearEntry = (BlackboardIntType*)blackboard.at(gear);

	currGear = gearEntry->GetValue();
	//brakeAmnt = static_cast<BlackboardFloatType*>(blackboard["brakeAmnt"])->GetValue();
	//accelAmnt = static_cast<BlackboardFloatType*>(blackboard["acceltAmnt"])->GetValue();

	/*Selector *root = new Selector();
	EdgeDistanceCheck *edgeCheck = new EdgeDistanceCheck();

	edgeCheck->AddChild(new AccelerationNode());
	root->AddChild(edgeCheck);

	root->AddChild(new Sequence());*/

	car->ctrl.brakeCmd = brakeAmnt;
	car->ctrl.accelCmd = accelAmnt;
	car->ctrl.gear = currGear;

	std::cout << "end driving the car!\n";
}
