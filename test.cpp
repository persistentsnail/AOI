#include <iostream>
#include <string>
#include <cstdlib>
#include "list_aoi.h"

struct Entity
{
	AOIEntity _aoi;
	int _id;
};

int main(int argc, char *argv[])
{
	int n = 256;
	if (argc >= 2)
		n = atoi(argv[1]);
	Entity *entities = new Entity[n];
	for (int i = 0; i < n; i++)
	{
		
	}
	return 0;
}
