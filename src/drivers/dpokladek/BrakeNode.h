#pragma once
#include "Behavior.h"
#include "AI.h"

#include <stdio.h>
#include <iostream>

class BrakeNode : public Behavior
{
	AI *ai;
	int m_iInitializeCalled;
	int m_iTerminateCalled;
	int m_iUpdateCalled;
	Status m_eReturnStatus;
	Status m_eTerminateStatus;

public:
	BrakeNode(AI *_ai)
		: m_iInitializeCalled(0)
		, m_iTerminateCalled(0)
		, m_iUpdateCalled(0)
		, m_eReturnStatus(BH_RUNNING)
		, m_eTerminateStatus(BH_INVALID)
		, ai(_ai)
	{}
	
	virtual ~BrakeNode()
	{

	}

	virtual void onInitialize()
	{
		++m_iInitializeCalled;
	}

	virtual void onTerminate(Status s)
	{
		++m_iTerminateCalled;
		m_eTerminateStatus = s;
	}

	virtual Status update() override
	{
		++m_iUpdateCalled;

		std::cout << "BrakeNode\n";
		BlackboardFloatType *brakeEntry = (BlackboardFloatType*)ai->blackboard.at(ai->brake);
		brakeEntry->SetValue(0.0f);

		return m_eReturnStatus;
	}
};

