#pragma once
#include "Blackboard.h"
#include "Behavior.h"
#include "AI.h"

#include <stdio.h>
#include <iostream>

class AccelerateNode : public Behavior
{
	AI *ai;
	int m_iInitializeCalled;
	int m_iTerminateCalled;
	int m_iUpdateCalled;
	Status m_eReturnStatus;
	Status m_eTerminateStatus;

public:
	AccelerateNode(AI *_ai)
		: m_iInitializeCalled(0)
		, m_iTerminateCalled(0)
		, m_iUpdateCalled(0)
		, m_eReturnStatus(BH_RUNNING)
		, m_eTerminateStatus(BH_INVALID)
		, ai(_ai)
	{}

	virtual ~AccelerateNode()
	{

	}

	virtual void onInitialize()
	{
		std::cout << "Accel init" << std::endl;
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

		std::cout << "AccelNode\n";
		BlackboardFloatType *accelEntry = (BlackboardFloatType*)ai->blackboard.at(ai->accel);
		accelEntry->SetValue(0.3f);

		return m_eReturnStatus;
	}
};

