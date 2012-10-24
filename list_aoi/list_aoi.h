#ifndef __LIST_AOI_H__
#define __LIST_AOI_H__

#include <list>

typedef enum {POINT, AREA} AOIType;

#define POINT_NODE_FLAG 1
#define AREA_NODE_FLAG 2
#define INIT_POS_VAL -127

class AOIEntity;
typedef void (*AOICallback)(AOITrigger *aoier, AOITrigger *entitier);  
struct TriggerNode
{
	int _x,_y;
	unsigned short _flag;
	TriggerNode *_next, *_prev;
	AOITrigger *_owner;
	TriggerNode(AOITrigger *owner, unsigned short flag):
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
		void OnTriggerAtX(TriggerNode *area_node, TriggerNode *point_node);
		void OnTriggerAtY(TriggerNode *area_node, TriggerNode *point_node);
	public:
		AOITrigger(AOIEntity *owner, AOIType type, 
					int left, int right, int top, int bottom)
		~AOITrigger();
		void Move(int xpos, int ypos);
		void Enter(int xpos, int ypos);
		void Leave();
};

class AOIEntity
{
		std::list<AOITrigger *> _triggers;
		bool _has_in;
	public:
		int _xpos, _ypos;
		AOIEntity():_xpos(-127), _ypos(-127), _has_in(false) {}
	 	AOITrigger *AddTrigger(AOIType type, int left, int right, int top, int bottom);
		void DelTrigger(AOITrigger *);
		void Move(int xpos, int ypos);
		void Enter(int xpos, int ypos);
		void Leave();
};

class AOIManager
{
	public:
		void Init(AOICallback enterCb, AOICallback leaveCb);
		void EntityEnter(AOIEntity *, int , int ,);
		void EntityLeave(AOIEntity *);
};
#endif
