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
	BlackboaordBoolType(bool val) : value(val) {}
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

