#ifndef __LIST_AOI_H__
#define __LIST_AOI_H__

#include <list>

typedef enum {POINT, AREA} AOIType;

#define POINT_NODE_FLAG 1
#define AREA_NODE_FLAG 2
#define INIT_POS_VAL -127

class AOIEntity;
class AOITrigger;
typedef void (*AOICallback)(AOIEntity *aoier, AOIEntity *entitier);  
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

		void MoveX();
		void MoveY();
		void OnTriggerAtX(TriggerNode *area_node, TriggerNode *point_node);
		void OnTriggerAtY(TriggerNode *area_node, TriggerNode *point_node);
	public:
		AOITrigger(AOIEntity *owner, AOIType type, 
					int left, int right, int top, int bottom);
		~AOITrigger();
		void Move(int xpos, int ypos);
		void Enter(int xpos, int ypos);
		void Leave();
		inline AOIType Type() { return _aoi_type; }
		inline int Left () { return _left; }
		inline int Right () { return _right; }
		inline int Top () { return _top; }
		inline int Bottom() { return _bottom; }

		inline bool XIsIn(int x)
		{ return x > _x_nodes[0]->_x && x < _x_nodes[1]->_x; } 
		inline bool XWasIn(int x)
		{ return x > _xcenter + _left && x < _xcenter + _right; }
		inline bool YIsIn(int y)
		{ return y > _y_nodes[0]->_y && y < _y_nodes[1]->_y; }
		inline bool YWasIn(int y)
		{ return y > _ycenter + _top && y < _ycenter + _bottom; }
};

class AOIEntity
{
		std::list<AOITrigger *> _triggers;
		bool _has_in;
		int _id;
	public:
		int _xpos, _ypos;
		AOIEntity(int id = -1):_id(id), _xpos(-127), _ypos(-127), _has_in(false) {}
		~AOIEntity();
	 	AOITrigger *AddTrigger(AOIType type, int left, int right, int top, int bottom);
		void DelTrigger(AOITrigger *);
		void Move(int xpos, int ypos);
		void Enter(int xpos, int ypos);
		void Leave();
		inline int GetId() const { return _id; }
		inline void SetId(int id) { _id = id; }


		const std::list<AOITrigger *> & Triggers() const
		{
			return _triggers;
		}
};

class AOIManager
{
	public:
		void Init(AOICallback enterCb, AOICallback leaveCb);
		void EntityEnter(AOIEntity *, int, int );
		void EntityLeave(AOIEntity *);
};
#endif
