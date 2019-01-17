#pragma once
#include "Behavior.h"
class ShiftUpNode : public Behavior
{
	AI *ai;
	tCarElt *car;

	float gr_up;

	int m_iInitializeCalled;
	int m_iTerminateCalled;
	int m_iUpdateCalled;
	Status m_eReturnStatus;
	Status m_eTerminateStatus;

public:
	ShiftUpNode(AI *_ai)
		: m_iInitializeCalled(0)
		, m_iTerminateCalled(0)
		, m_iUpdateCalled(0)
		, m_eReturnStatus(BH_RUNNING)
		, m_eTerminateStatus(BH_INVALID)
		, ai(_ai)
	{}

	virtual ~ShiftUpNode() {}

	virtual void onInitialize()
	{
		++m_iInitializeCalled;
		car = ai->carReference;
	}

	virtual void onTerminate(Status s)
	{
		++m_iTerminateCalled;
		m_eTerminateStatus = s;
	}

	virtual Status update() override
	{
		++m_iUpdateCalled;

		BlackboardIntType *gearEntry = (BlackboardIntType*)ai->blackboard.at(ai->gearKey);
		BlackboardBoolType *stuckEntry = (BlackboardBoolType*)ai->blackboard.at(ai->stuckKey);

		if (stuckEntry->GetValue() == false)
		{
			if (car->_gear <= 0)
			{
				gearEntry->SetValue(1);
				return BH_SUCCESS;
			}

			gr_up = car->_gearRatio[car->_gear + car->_gearOffset];
			float omega = car->_enginerpmRedLine / gr_up;
			float wr = car->_wheelRadius(2);

			if (omega*wr*ai->SHIFT < car->_speed_x)
			{
				gearEntry->SetValue(car->_gear + 1);
				return BH_SUCCESS;
			}

			return BH_FAILURE;
		}
		else
		{
			return BH_FAILURE;
		}

		return BH_INVALID;
	}
};

