#pragma once
#include "Behavior.h"

#include <robot.h>
#include <robottools.h>
#include <stdio.h>
#include <iostream>

class SteeringClass : public Behavior
{
public:
	AI *ai;
	tCarElt *car;

	int m_iInitializeCalled;
	int m_iTerminateCalled;
	int m_iUpdateCalled;
	Status m_eReturnStatus;
	Status m_eTerminateStatus;

	/* Steering variables */
	float angle;
	const float SC = 1.0;

	SteeringClass(AI *_ai)
		: m_iInitializeCalled(0)
		, m_iTerminateCalled(0)
		, m_iUpdateCalled(0)
		, m_eReturnStatus(BH_RUNNING)
		, m_eTerminateStatus(BH_INVALID)
		, ai(_ai)
	{}

	virtual ~SteeringClass() {}

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

		//float trackAngle = RtTrackSideTgAngleL(&(car->_trkPos));
		//angle = trackAngle - car->_yaw;
		//NORM_PI_PI(angle);

		//float _steerAngle = angle - car->_trkPos.toMiddle / car->_trkPos.seg->width;
		//steerEntry->SetValue(_steerAngle);

		float targetAngle;
		v2d target = ai->getTargetPoint();

		targetAngle = atan2(target.y - car->_pos_Y, target.x - car->_pos_X);
		targetAngle -= car->_yaw;

		NORM_PI_PI(targetAngle);

		steerEntry->SetValue(targetAngle);

		return m_eReturnStatus;
	}
};

