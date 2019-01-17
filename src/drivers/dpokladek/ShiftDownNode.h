#pragma once
#include "Behavior.h"
class ShiftDownNode : public Behavior
{
	AI *ai;
	tCarElt *car;

	float gr_down;

	int m_iInitializeCalled;
	int m_iTerminateCalled;
	int m_iUpdateCalled;
	Status m_eReturnStatus;
	Status m_eTerminateStatus;

public:
	ShiftDownNode(AI *_ai)
		: m_iInitializeCalled(0)
		, m_iTerminateCalled(0)
		, m_iUpdateCalled(0)
		, m_eReturnStatus(BH_RUNNING)
		, m_eTerminateStatus(BH_INVALID)
		, ai(_ai)
	{}

	virtual ~ShiftDownNode() {}

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
			gr_down = car->_gearRatio[car->_gear + car->_gearOffset - 1];
			float omega = car->_enginerpmRedLine / gr_down;
			float wr = car->_wheelRadius(2);

			if (car->_gear > 1 && omega*wr*ai->SHIFT > car->_speed_x + ai->SHIFT_MARGIN)
			{
				gearEntry->SetValue(car->_gear - 1);
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

