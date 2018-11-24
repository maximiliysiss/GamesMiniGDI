#include "GravitySystem.h"


void GravitySystem::Mathematic() // Математика гравитации и движения
{
	std::vector<Object*> temp_obj(objects_gr); // Объекты
	int index = 0;
	for (auto i = objects_gr.begin(); i != objects_gr.end(); i++, index++)
	{
		if ((*i)->Gravity && (*i)->type!=Object::TYPE::RECT)
		{
			(*i)->Movement(); // Двжиение
			(*i)->ForceGravity(temp_obj, index); // Столкновения
		}
	}
}