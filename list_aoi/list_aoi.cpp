#include "list_aoi.h"

static AxisList g_xy_Axes;

static void InsertAfter(TriggerNode *pos, TriggerNode *node)
{
	node->_prev = pos;
	node->_next = pos->_next;
	pos->_next  = node;
}

AOITrigger::AOITrigger(AOIEntity *owner, AOIType type,
	int left, int right, int top, int bottom):
	_owner(owner), _aoi_type(type), _left(left),
	_right(right), _top(top), _bottom(bottom)
	{
		_xcenter = _ycenter = INIT_POS_VAL;
		if (_aoi_type == POINT)
		{
			_x_nodes[0] = new TriggerNode(_owner, POINT_NODE_FLAG);
			_x_nodes[0]->_x = _xcenter;

			_x_nodes[1] = NULL;
			_y_nodes[0] = new TriggerNode(_owner, POINT_NODE_FLAG);
			_y_nodes[0]->_y = _ycenter;
			_y_nodes[1] = NULL;
		}
		else if (_aoi_type == AREA)
		{
			_x_nodes[0] = new TriggerNode(_owner, AREA_NODE_FLAG);
			_x_nodes[0]->_x = INIT_POS_VAL;
			_x_nodes[1] = new TriggerNode(_owner, AREA_NODE_FLAG);
			_x_nodes[1]->_x = INIT_POS_VAL;

			_y_nodes[0] = new TriggerNode(_owner, AREA_NODE_FLAG);
			_y_nodes[0]->_y = INIT_POS_VAL;
			_y_nodes[1] = new TriggerNode(_owner, AREA_NODE_FLAG);
			_y_nodes[1]->_y = INIT_POS_VAL;
		}
		for (int i = 0; i < 2; i++)
		{
			if (_x_nodes[i])
				InsertAfter(g_xy_Axes.x_nodes_header, _x_nodes[i]);
			if (_y_nodes[i])
				InserAfter(g_xy_Axes.y_nodes_header, _y_nodes[i]);
		}
	}

void AOITrigger::Move(int xpos, int ypos)
{
}
