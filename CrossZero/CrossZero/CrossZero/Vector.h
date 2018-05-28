#pragma once
#include <Windows.h>
template<class T>
class Vector
{
	int size;
	struct Element
	{
		Element(int v) :value(v) { next = NULL; }
		T value;
		Element* next;
	};
	Element* first;
public:
	Vector()
	{
		size = 0;
		first = NULL;
	}
	Vector(T v)
	{
		size = 1;
		first = new Element(v);
	}
	Vector(T v, int count);
	void push_back(T v);
	void clear();
	int GetSize()
	{
		return size;
	}
	void erase(int index);
	T operator[](int index)
	{
		if (index >= size)
			return 0;
		Element * temp = first;
		for (int i = 0; i < index; i++)
			temp = temp->next;
		return temp->value;
	}
};