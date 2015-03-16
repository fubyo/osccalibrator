#ifndef __POOL_H__
#define __POOL_H__

#include "..\juce\juce_amalgamated.h"
#include <map>
using namespace std;

struct ObjectValue
{
	void* object;
	double value;

	ObjectValue()
	{
		object = 0;
		value = 0;
	};
};

class Pool
{
	static ScopedPointer<Pool> instance;
	Pool();

	static map<String, ObjectValue> map;

public:
	
	static ScopedPointer<Pool> Instance();
	double getValue(String key);
	void* getObject(String key);
	void reg(String key,void* object);
	void reg(String key,double value);
	void unreg(String key);
};

#endif