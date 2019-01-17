#pragma once
#include "Behavior.h"
#include "AI.h"

#include <robot.h>
#include <robottools.h>
#include <stdio.h>
#include <iostream>

class StuckNode : public Behavior
{
	AI *ai;
	tCarElt *car;

	int m_iInitializeCalled;
	int m_iTerminateCalled;
	int m_iUpdateCalled;
	Status m_eReturnStatus;
	Status m_eTerminateStatus;

	float angle;
	const float SC = 1.0;

public:
	StuckNode(AI *_ai)
		: m_iInitializeCalled(0)
		, m_iTerminateCalled(0)
		, m_iUpdateCalled(0)
		, m_eReturnStatus(BH_RUNNING)
		, m_eTerminateStatus(BH_INVALID)
		, ai(_ai)
	{}

	virtual ~StuckNode() {}

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

		BlackboardFloatType *steerEntry = (BlackboardFloatType*)ai->blackboard.at(ai->steerKey);
		BlackboardIntType *gearEntry	= (BlackboardIntType*)ai->blackboard.at(ai->gearKey);
		BlackboardFloatType *accelEntry = (BlackboardFloatType*)ai->blackboard.at(ai->accelKey);
		BlackboardFloatType *brakeEntry = (BlackboardFloatType*)ai->blackboard.at(ai->brakeKey);
		BlackboardBoolType *stuckEntry	= (BlackboardBoolType*)ai->blackboard.at(ai->stuckKey);

		float trackAngle = RtTrackSideTgAngleL(&(car->_trkPos));
		angle = trackAngle - car->_yaw;
		NORM_PI_PI(angle);

		float _steerAngle = angle - car->_trkPos.toMiddle / car->_trkPos.seg->width;

		if (ai->isStuck())
		{
			stuckEntry->SetValue(true);
			steerEntry->SetValue(-_steerAngle);
			gearEntry->SetValue(-1);
			accelEntry->SetValue(0.5);
			brakeEntry->SetValue(0.0);

			return BH_SUCCESS;
		}
		else
		{
			stuckEntry->SetValue(false);
			return BH_FAILURE;
		}

		return BH_INVALID;
	}
};

