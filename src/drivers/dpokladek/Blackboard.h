#pragma once

#include <robot.h>
#include <robottools.h>

class Blackboard
{
public:
	Blackboard();
	~Blackboard();
};

class BlackboardBaseType
{
public:
	BlackboardBaseType() {};
	virtual ~BlackboardBaseType() {};
};

class BlackboardBoolType : public BlackboardBaseType 
{
public:
	BlackboardBoolType(bool val) : value(val) {}
	bool GetValue() { return value; }
	void SetValue(bool val) { value = val; }

private:
	bool value;
};

class BlackboardIntType : public BlackboardBaseType
{
public:
	BlackboardIntType(int val) : value(val) {}
	int GetValue() { return value; }
	void SetValue(int val) { value = val; }

private:
	int value;
};

class BlackboardFloatType : public BlackboardBaseType
{
public:
	BlackboardFloatType(float val) : value(val) {}
	float GetValue() { return value; }
	void SetValue(float val) { value = val; }

private:
	float value;
};

class BlackboardCarType : public BlackboardBaseType
{
public:
	BlackboardCarType(tCarElt car) : car(car) {}
	tCarElt GetCar() { return car; }
	void SetCar(tCarElt _car) { car = _car; }

private:
	tCarElt car;
};

