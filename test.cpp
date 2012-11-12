#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <time.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include "list_aoi.h"

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <list>

using  std::list;

#define MAX_TRIGGERS 100

#define R 8
#define W 1024
#define H 768

#define MAX_ENTITY_NUMBER 1024
static int Entity_Num = 0;
static AOIEntity Entities[MAX_ENTITY_NUMBER];

/* for test */
struct Rect
{
	int _left, _right, _top, _bottom;
};

struct Point
{
	int _x, _y;
};

struct TestEntity
{
	Rect _area;
	Point _pt;
	Point _old_pt;
	int _id;

	inline bool PtInArea(const Point &pt, const Point &center) const
	{
		return ( pt._x > _area._left + center._x && 
				 pt._x < _area._right + center._x &&
				 pt._y > _area._top + center._y &&
				 pt._y < _area._bottom + center._y
				 );

	}

	inline bool WasIn(const Point &pt) const
	{
		return PtInArea(pt, _old_pt);
	}

	inline bool IsIn(const Point &pt) const
	{
		return PtInArea(pt, _pt);
	}
};

TestEntity TestEntities[MAX_ENTITY_NUMBER];

int LeaveResult[MAX_ENTITY_NUMBER];
int EnterResult[MAX_ENTITY_NUMBER];


inline void Process(const TestEntity &area_entity, const TestEntity &pt_entity)
{
	if (area_entity.WasIn(pt_entity._old_pt) && !area_entity.IsIn(pt_entity._pt))
		/* pt leave area */
		assert(LeaveResult[area_entity._id] == pt_entity._id);
	else if (!area_entity.WasIn(pt_entity._old_pt) && area_entity.IsIn(pt_entity._pt))
		/* pt enter area */
		assert(EnterResult[area_entity._id] == pt_entity._id);
}

void TestMove(int index, int x, int y)
{
	TestEntity &test = TestEntities[index];
	test._pt._x = x;
	test._pt._y = y;

	for (int i = 0; i < Entity_Num; i++)
	{
		if (i == index) continue;
		Process(test, TestEntities[i]);
		Process(TestEntities[i], test);
	}
	test._old_pt._x = x;
	test._old_pt._y = y;
}

void EnterCallback(AOIEntity *aoier, AOIEntity *entitier)
{
	EnterResult[aoier->GetId()] = entitier->GetId();
}


void LeaveCallback(AOIEntity *aoier, AOIEntity *entitier)
{
	LeaveResult[aoier->GetId()] = entitier->GetId();
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

void timercb(int)
{
	int * rarray = new int[Entity_Num];
	Shuffle(rarray, Entity_Num);
	int active_size = Entity_Num / 3;

	for (int i = 0; i < active_size; i++)
	{
		int x = random()%W;
		int y = random()%H;
		int index = rarray[i];
		Entities[index].Move(x, y);
		TestMove(index, x, y);
	}
	delete [] rarray;
	
	alarm(1);
}

int main(int argc, char *argv[])
{
	int i = 0;
	srandom(time(NULL));

	if (argc >= 2)
		Entity_Num = atoi(argv[1]);
	for (i = 0; i < Entity_Num; i++)
	{
		Entities[i].AddTrigger(POINT, -1, -1, -1, -1); 
		Entities[i].SetId(i);
	}

	i = 0;
	while (i < Entity_Num)
	{
		unsigned int rval = random() % 0xffffffff;
		for (int j = 0; j < 32 && i < Entity_Num; j++, i++)
		{
			if (rval & 1)
				Entities[i].AddTrigger(AREA,-R, R, -R, R); 
			rval >>= 1;
		}
	}

	AOIManager scene_mgr;
	scene_mgr.Init(EnterCallback, LeaveCallback);

	for (i = 0; i < Entity_Num; i++)
	{
		scene_mgr.EntityEnter(&Entities[i], random()%W, random()%H);
	}
	
	for (int i = 0; i < Entity_Num; i++)
	{
		AOIEntity & aoi_entity = Entities[i];
		list<AOITrigger *> triggers = aoi_entity.Triggers();
		list<AOITrigger *>::iterator iter = triggers.begin();

		/* 现在测试只支持一个Entity挂两个AOI，一个是Area AOI，一个是Point AOI */
		for (int j = 0; j < 2; j++)
		{
			if ((*iter)->Type() == AREA)
			{
				TestEntities[i]._area._left = (*iter)->Left();
				TestEntities[i]._area._right = (*iter)->Right();
				TestEntities[i]._area._top = (*iter)->Top();
				TestEntities[i]._area._bottom = (*iter)->Bottom();
			}
			else if ((*iter)->Type() == POINT)
			{
				TestEntities[i]._old_pt._x = aoi_entity._xpos;
				TestEntities[i]._old_pt._y = aoi_entity._ypos;
				TestEntities[i]._pt._x = aoi_entity._xpos;
				TestEntities[i]._pt._y = aoi_entity._ypos;
			}
			iter++;
		}
		TestEntities[i]._id = i;
	}

	memset(&LeaveResult, -1, sizeof(LeaveResult));
	memset(&EnterResult, -1, sizeof(EnterResult));

	struct sigaction act;
	act.sa_handler = timercb;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;

	if (sigaction(SIGALRM, &act, NULL) < 0)
	{
		perror("sigaction ");
		exit(-1);
	}

	alarm(1); 
	while (1)
		pause();
	return 0;
}
