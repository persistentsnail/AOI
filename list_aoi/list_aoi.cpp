#include "list_aoi.h"

static AxisList g_xy_Axes;
static AOICallback g_leaveCb;
static AOICallback g_enterCb;

static void InsertAfter(TriggerNode *pos, TriggerNode *node)
{
	node->_prev = pos;
	node->_next = pos->_next;
	pos->_next  = node;
}

static void SwapNode(TriggerNode *left, TriggerNode *right)
{
	left->_next = right->_next;
	right->_prev = left->_prev;
	left->_prev = right;
	right->_next = left;
}

AOITrigger::AOITrigger(AOIEntity *owner, AOIType type,
	int left, int right, int top, int bottom):
	_owner(owner), _aoi_type(type), _left(left),
	_right(right), _top(top), _bottom(bottom)
	{
		_xcenter = _ycenter = INIT_POS_VAL;
		for (int i = 0; i < 2; i++)
		{
			if (_aoi_type == POINT)
			{
				if (i == 0)
				{
					_x_nodes[0] = new TriggerNode(this, POINT_NODE_FLAG);
					_x_nodes[0]->_x = INIT_POS_VAL;
					
					_y_nodes[0] = new TriggerNode(this, POINT_NODE_FLAG);
					_y_nodes[0]->_y = INIT_POS_VAL;
				}
				else
				{
					_x_nodes[1] = NULL;
					_y_nodes[1] = NULL;
				}
			}
			else if (_aoi_type == AREA)
			{
				_x_nodes[i] = new TriggerNode(this, AREA_NODE_FLAG);
				_y_nodes[i] = new TriggerNode(this, AREA_NODE_FLAG);
			}

			if (_x_nodes[i])
				InsertAfter(g_xy_Axes.x_nodes_header, _x_nodes[i]);
			if (_y_nodes[i])
				InserAfter(g_xy_Axes.y_nodes_header, _y_nodes[i]);
		}
	}

void AOITrigger::OnTriggerAtX(TriggerNode *area_node, TriggerNode *point_node)
{
	bool ret = area_node->_owner->XWasIn(point_node->_x);
	if (!ret) return;
	if (area_node->_owner->YWasIn(point_node->_owner->_ycenter)) // node leave area
	{
		g_leaveCb(area_node->_owner, point_node->_owner);
	}
	else if (area_node->_owner->YIsIn(point_node->_y) && area_node->_owner->XIsIn(point_node->_x)
}

void AOITrigger::MoveX(int xpos, int ypos)
{
	for (int i = 0; i < 2; i++)
	{
		TriggerNode *loop = _x_nodes[i];
		if (!loop)
			continue;
		while (loop && loop->_prev && loop->_prev->_x > loop->_x)
		{
			if ((loop->_prev->_flag & AREA_NODE_FLAG) && (loop->_flag & POINT_NODE_FLAG)) 
				OnTriggerAtX(loop->_prev, loop);
			else if ((loop->_prev->_flag & POINT_NODE_FLAG) && (loop->_flag & AREA_NODE_FL))
				OnTriggerAtX(loop, loop->_prev);
			SwapNode(loop->_prev, loop);
		}
		while (loop && loop->_next && loop->_next->_x < loop->_x)
		{
			if ((loop->_next->_flag & AREA_NODE_FLAG) && (loop->_flag & POINT_NODE_FLAG))
				OnTriggerAtX(loop->_next, loop);
			else if ((loop->_next->_flag & POINT_NODE_FLAG) && (loop->_flag & AREA_NODE_FLAG))
				OnTriggerAtX(loop, loop->_next);
			SwapNode(loop, loop->_next);
		}
	}
}

void AOITrigger::Move(int xpos, int ypos)
{
	if (_aoi_type == POINT)
	{
		_x_nodes[0]->_x = xpos;
		_y_nodes[0]->_y = ypos;
	}
	else if (_aoi_type == AREA)
	{
		_x_nodes[0]->_x = xpos + _left;
		_x_nodes[1]->_x = xpos + _right;
		_y_nodes[0]->_y = ypos + _top;
		_y_nodes[1]->_y = ypos + _bottom;
	}
}
