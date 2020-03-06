#ifndef __IDENTIFIER_H_PAVEL
#define __IDENTIFIER_H_PAVEL

#include <map>
#include <set>
#include <pthread.h>

class Identifier
{
public:
	explicit Identifier(int n = 0) : n(n)
	{}

	int GetId();
	void Delete(int id);
private:
	std::set<int> indexes;
	int n;
};


#endif 
