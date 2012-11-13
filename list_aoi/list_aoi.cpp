#include <stdio.h>
#include "list_aoi.h"

static AxisList g_xy_Axes;
static AOICallback g_leaveCb;
static AOICallback g_enterCb;

static void InsertAfter(TriggerNode *pos, TriggerNode *node)
{
	node->_prev = pos;
	node->_next = pos->_next;
	if (pos->_next)
		pos->_next->_prev  = node;
	pos->_next = node;
}

static void SwapNode(TriggerNode *left, TriggerNode *right)
{
	left->_next = right->_next;
	right->_prev = left->_prev;
	left->_prev = right;
	right->_next = left;
	if (right->_prev)
		right->_prev->_next = right;
	if (left->_next)
		left->_next->_prev = left;
}

static void RemoveNode(TriggerNode *node)
{
	if (node->_prev)
		node->_prev->_next = node->_next;
	if (node->_next)
		node->_next->_prev = node->_prev;
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
				_x_nodes[i]->_x = INIT_POS_VAL;
				_y_nodes[i] = new TriggerNode(this, AREA_NODE_FLAG);
				_y_nodes[i]->_y = INIT_POS_VAL;
			}
		}
	}

AOITrigger::~AOITrigger()
{
	for (int i = 0; i < 2; i++)
	{
		if (_x_nodes[i])
			delete _x_nodes[i];
		if (_y_nodes[i])
			delete _y_nodes[i];
	}
}

void AOITrigger::OnTriggerAtX(TriggerNode *area_node, TriggerNode *point_node)
{
	if (area_node->_owner->_owner == point_node->_owner->_owner)
		return;
	bool ret = area_node->_owner->YWasIn(point_node->_owner->_ycenter);
	if (!ret) return;
	if (area_node->_owner->XWasIn(point_node->_owner->_xcenter)) // node leave area
	{
		g_leaveCb(area_node->_owner->_owner, point_node->_owner->_owner);
	}
	else if (area_node->_owner->YIsIn(point_node->_y))
	{
		g_enterCb(area_node->_owner->_owner, point_node->_owner->_owner);
	}
}

void AOITrigger::OnTriggerAtY(TriggerNode *area_node, TriggerNode *point_node)
{
	if (area_node->_owner->_owner == point_node->_owner->_owner)
		return;

	bool ret = area_node->_owner->XIsIn(point_node->_x);
	if (!ret) return;
	if (area_node->_owner->YIsIn(point_node->_y))
	{
		g_enterCb(area_node->_owner->_owner, point_node->_owner->_owner);
	}
	else if (area_node->_owner->XWasIn(point_node->_owner->_xcenter))
	{
		g_leaveCb(area_node->_owner->_owner, point_node->_owner->_owner);
	}
}

void AOITrigger::MoveX()
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
			else if ((loop->_prev->_flag & POINT_NODE_FLAG) && (loop->_flag & AREA_NODE_FLAG))
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

void AOITrigger::MoveY()
{
	for (int i = 0; i < 2; i++)
	{
		TriggerNode *loop = _y_nodes[i];
		if (!loop)
			continue;
		while (loop && loop->_prev && loop->_prev->_y > loop->_y)
		{
			if ((loop->_prev->_flag & AREA_NODE_FLAG) && (loop->_flag & POINT_NODE_FLAG))
				OnTriggerAtY(loop->_prev, loop);
			else if ((loop->_prev->_flag & POINT_NODE_FLAG) && (loop->_flag & AREA_NODE_FLAG))
				OnTriggerAtY(loop, loop->_prev);
			SwapNode(loop->_prev, loop);
		}
		while (loop && loop->_next && loop->_next->_y < loop->_y)
		{
			if ((loop->_next->_flag & AREA_NODE_FLAG) && (loop->_flag & POINT_NODE_FLAG))
				OnTriggerAtY(loop->_next, loop);
			else if ((loop->_next->_flag & POINT_NODE_FLAG) && (loop->_flag & AREA_NODE_FLAG))
				OnTriggerAtY(loop, loop->_next);
			SwapNode(loop, loop->_next);
		}
	}
}

void AOITrigger::Move(int xpos, int ypos)
{
	if (_aoi_type == POINT)
	{
		_x_nodes[0]->_x = xpos;
		_x_nodes[0]->_y = ypos;
		_y_nodes[0]->_y = ypos;
		_y_nodes[0]->_x = xpos;
	}
	else if (_aoi_type == AREA)
	{
		_x_nodes[0]->_x = xpos + _left;
		_x_nodes[1]->_x = xpos + _right;
		_y_nodes[0]->_y = ypos + _top;
		_y_nodes[1]->_y = ypos + _bottom;
	}
	MoveX();
	MoveY();
	_xcenter = xpos;
	_ycenter = ypos;
}

void AOITrigger::Enter(int xpos, int ypos)
{
	for (int i = 0; i < 2; i++)
	{
		if (_x_nodes[i])
			InsertAfter(g_xy_Axes.x_nodes_header, _x_nodes[i]);
		if (_y_nodes[i])
			InsertAfter(g_xy_Axes.y_nodes_header, _y_nodes[i]);
	}
	Move(xpos, ypos);
}

void AOITrigger::Leave()
{
	Move(-127, -127);
	for (int i = 0; i < 2; i++)
	{		
		if (_x_nodes[i])
			RemoveNode(_x_nodes[i]);
		if (_y_nodes[i])
			RemoveNode(_y_nodes[i]);
	}
}

AOITrigger * AOIEntity::AddTrigger(AOIType type, int left, int right, int top, int bottom)
{
	AOITrigger *trigger = new AOITrigger(this, type, left, right, top, bottom);
	_triggers.push_back(trigger);
	if (_has_in)
		trigger->Enter(_xpos, _ypos);
	return trigger;
}

void AOIEntity::DelTrigger(AOITrigger *trigger)
{
	_triggers.remove(trigger);
	if (_has_in)
		trigger->Leave();
	delete trigger;
}

void AOIEntity::Enter(int xpos, int ypos)
{
	if (_has_in) return;
	std::list<AOITrigger *>::iterator iter = _triggers.begin();
	for (; iter != _triggers.end(); iter++)
		(*iter)->Enter(xpos, ypos);
	_xpos = xpos;
	_ypos = ypos;
	_has_in = true;
}

void AOIEntity::Leave()
{
	if (!_has_in) return;
	std::list<AOITrigger *>::iterator iter = _triggers.begin();
	for (; iter != _triggers.end(); iter++)
		(*iter)->Leave();
	_has_in = false;
}

void AOIEntity::Move(int xpos, int ypos)
{
	if (!_has_in) return;
	std::list<AOITrigger *>::iterator iter = _triggers.begin();
	for (; iter != _triggers.end(); iter++)
		(*iter)->Move(xpos, ypos);
	_xpos = xpos;
	_ypos = ypos;
}

AOIEntity::~AOIEntity()
{
	std::list<AOITrigger *>::iterator iter = _triggers.begin();
	for (; iter != _triggers.end(); iter++)
		DelTrigger(*iter);
}

void AOIManager::Init(AOICallback enterCb, AOICallback leaveCb)
{
	g_xy_Axes.x_nodes_header = new TriggerNode(NULL, 0);
	g_xy_Axes.y_nodes_header = new TriggerNode(NULL, 0);
	g_xy_Axes.x_nodes_header->_x = g_xy_Axes.x_nodes_header->_y = INIT_POS_VAL - 1;
	g_xy_Axes.y_nodes_header->_x = g_xy_Axes.x_nodes_header->_y = INIT_POS_VAL - 1;
	g_leaveCb = leaveCb;
	g_enterCb = enterCb;
}

void AOIManager::EntityEnter(AOIEntity *entity, int x, int y)
{
	entity->Enter(x, y);
}

void AOIManager::EntityLeave(AOIEntity *entity)
{
	entity->Leave();
}
