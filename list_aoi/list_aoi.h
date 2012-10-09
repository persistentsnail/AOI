#ifndef __LIST_AOI_H__
#define __LIST_AOI_H__

#include <list>

typedef enum {POINT, AREA} AOIType;

#define POINT_NODE_FLAG 1
#define AREA_NODE_FLAG 2
#define INIT_POS_VAL -127

class AOIEntity;
struct TriggerNode
{
	int _x,_y;
	unsigned short _flag;
	TriggerNode *_next, *_prev;
	AOIEntity *_owner;
	TriggerNode(AOIEntity *owner, unsigned short flag):
		_owner(owner), _flag(flag),_next(NULL),_prev(NULL) {}
};

struct AxisList
{
	TriggerNode *x_nodes_header;
	TriggerNode *y_nodes_header;
};


class AOITrigger
{
		AOIEntity *_owner;
		AOIType _aoi_type;
		int _left,_right,_top,_bottom;

		int _xcenter, _ycenter;
		TriggerNode * _x_nodes[2]; /* left,right */
		TriggerNode * _y_nodes[2]; /* top,bottom */

		void MoveX(int xpos, int ypos);
		void MoveY(int xpos, int ypos);
		void OnTriggerAtX();
		void OnTriggerAtY();
	public:
		AOITrigger(AOIEntity *owner, AOIType type, 
					int left, int right, int top, int bottom); 
		void Move(int xpos, int ypos);
};

class AOIEntity
{
		std::list<AOITrigger *> _triggers;
	public:
		int _xpos, _ypos;
		AOIEntity():_xpos(-127), _ypos(-127) {}
	 	AOITrigger *AddTrigger(AOIType type, int left, int right, int top, int bottom);
		void DelTrigger(AOITrigger *);
		void Move(int xpos, int ypos);
		/* entitier enter into (leave away) aoier */
		void EnterCallback(AOIEntity *aoier, AOIEntity *entitier);
		void LeaveCallback(AOIEntity *aoier, AOIEntity *entitier);
};

class AOIManager
{
	public:
		void Init();
		void AddEntity(AOIEntity *);
		void DelEntity(AOIEntity *);
};
#endif
