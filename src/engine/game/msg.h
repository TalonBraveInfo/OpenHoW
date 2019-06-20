#pragma once

struct NetMessage
{
	enum { SET_TICK, SET_PROPERTY } type;
	unsigned int actor_idx;
	char property_name[32];
	unsigned char property_value[16];
};
