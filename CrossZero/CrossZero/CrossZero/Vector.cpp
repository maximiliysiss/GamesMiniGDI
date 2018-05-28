#include "Vector.h"

template<class T>
Vector::Vector(T v, int count)
{
	size = count;
	first = new Element(4);
	Element * temp = first;
	for (int i = 0; i < count - 1; i++)
	{
		temp->next = new Element(v);
		temp = temp->next;
	}
}

template<class T>
void Vector::push_back(T v)
{
	size++;
	if (first == NULL)
	{
		first = new Element(v);
		return;
	}
	Element * temp = first;
	while (temp->next != NULL)
		temp = temp->next;
	temp->next = new Element(v);
}

template<class T>
void Vector::erase(int index)
{
	if (index > size)
		return;
	if (index == 0)
	{
		size--;
		Element * temp = first;
		first = first->next;
		delete temp;
		return;
	}
	if (index == size - 1)
	{
		Element *temp = first;
		for (int i = 0; i < index - 1; i++)
			temp = temp->next;
		delete temp->next;
		temp->next = NULL;
		size--;
		return;
	}
	Element * temp = first;
	for (int i = 0; i < index - 1; i++)
		temp = temp->next;
	Element * tt = temp->next->next;
	delete temp->next;
	temp->next = tt;
	size--;
}

template<class T>
void Vector::clear()
{
	Element * temp = first;
	first = NULL;
	while (temp != NULL)
	{
		Element * tt = temp->next;
		delete temp;
		temp = tt;
	}
	size = 0;
}