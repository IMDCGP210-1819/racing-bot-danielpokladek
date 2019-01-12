#pragma once
#include "Blackboard.h"
#include "Behavior.h"
#include "AI.h"

class AccelerateNode : public Behavior
{
	int m_iInitializeCalled;
	int m_iTerminateCalled;
	int m_iUpdateCalled;
	Status m_eReturnStatus;
	Status m_eTerminateStatus;

public:
	AI ai;

	AccelerateNode()
		: m_iInitializeCalled(0)
		, m_iTerminateCalled(0)
		, m_iUpdateCalled(0)
		, m_eReturnStatus(BH_RUNNING)
		, m_eTerminateStatus(BH_INVALID)
	{}

	virtual ~AccelerateNode()
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

		BlackboardFloatType *accelEntry = (BlackboardFloatType*)ai.blackboard.at(ai.accel);
		accelEntry->SetValue(1.0f);

		return m_eReturnStatus;
	}
};

