#pragma once
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

class BlackboaordBoolType : public BlackboardBaseType 
{
public:
	bool BlackboardBoolType(bool val) : value(val) {};
	bool GetValue() { return value; }
	void SetValue(bool val) { value = val; }

private:
	bool value;
};

