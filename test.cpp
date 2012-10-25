#include <iostream>
#include <string>
#include <cstdlib>
#include <time.h>
#include "list_aoi.h"

#define MAX_TRIGGERS 100
struct Entity
{
	AOIEntity _aoi;
	int _id;
};

#define R 8
#define W 1024
#define H 768
void EnterCallback(AOITrigger *aoier, AOIManager *entitier)
{
}


void LeaveCallback(AOITrigger *aoier, AOIManager *entitier)
{
}

void Shuffle(int *array, int n)
{
	for (int i = 0; i < n; i++)
		array[i] = i;
	for (int i = 0; i < n-1; i++)
	{
		int r = random()%(n-i) + i;
		int t = array[i];
		array[i] = array[r];
		array[r] = t;
	}
}

int main(int argc, char *argv[])
{
	int n = 256;
	int i = 0;
	srandom(time(NULL));
	if (argc >= 2)
		n = atoi(argv[1]);
	Entity *entities = new Entity[n];
	for (i = 0; i < n; i++)
	{
		entities[i]._aoi.AddTrigger(POINT, -1, -1, -1, -1); 
		entities[i]._id = i;
	}

	i = 0;
	while (i < n)
	{
		unsigned int rval = random() % 0xffffffff
		for (int j = 0; j < 32 && i < n; j++)
		{
			if (rval & 1)
				entities[i]._aoi.AddTrigger(AREA,-R, R, -R, R); 
			rval >>= 1;
		}
	}

	AOIManager scene_mgr;
	scene_mgr.Init(EnterCallback, LeaveCallback);

	for (i = 0; i < n; i++)
	{
		scene_mgr.EntityEnter(&entities[i]._aoi, random()%W, random()%H);
	}

	int *rarray = new int[n];
	while(1)
	{
		Shuffle(rarray, n);
		for (i = 0; i < 5; i++)
			scene_mgr.EntityEnter(&entities[i]._aoi, random()%W, random()%H);
		for (i = 5; i < 27; i++)
			entities[i]._aoi.Move(random()%W, random()%H);
		for (i = 27; i < 32; i++)
			scene_mgr.EntityLeave(&entities[i]._aoi);
		sleep(1);
	}
	delete [] rarray;
	delete [] entities;
	return 0;
}
