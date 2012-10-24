#include <iostream>
#include <string>
#include <cstdlib>
#include "list_aoi.h"

#define MAX_TRIGGERS 100
struct Entity
{
	AOIEntity _aoi;
	int _id;
	AOITrigger *_triggers[MAX_TRIGGERS];
	int _ntriggers;
	Entity():
};

int main(int argc, char *argv[])
{
	int n = 256;
	if (argc >= 2)
		n = atoi(argv[1]);
	Entity *entities = new Entity[n];
	for (int i = 0; i < n; i++)
	{
		entities[i]->_aoi.AddTrigger(POINT, -1, -1, -1, -1); 
	}
	return 0;
}
