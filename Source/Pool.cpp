#include "Pool.h"

ScopedPointer<Pool> Pool::instance = 0;
map<String, ObjectValue> Pool::map;

Pool::Pool()
{

}

ScopedPointer<Pool> Pool::Instance()
{
	if (!instance)
		instance = new Pool();
    return instance;
}

double Pool::getValue(String key)
{
	return map[key].value;
}

void* Pool::getObject(String key)
{
	return map[key].object;
}

void Pool::reg(String key, void* object)
{
	ObjectValue newItem;
	newItem.object = (void*) object;
	newItem.value = 0;

	map[key] = newItem;
}

void Pool::reg(String key, double value)
{
	ObjectValue newItem;
	newItem.object = 0;
	newItem.value = value;

	map[key] = newItem;
}

void Pool::unreg(String key)
{
	map.erase(map.find(key));
}