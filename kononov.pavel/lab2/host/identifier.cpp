#include "identifier.h"

void Identifier::Delete(int id)
{
	indexes.erase(id);
}


int Identifier::GetId()
{
	for (int id = 0; id < n; id++)
	{
		if(indexes.find(id) == indexes.end())
		{
			indexes.insert(id);
			return id;
		}
	}
	return -1;
}
