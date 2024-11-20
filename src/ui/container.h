#pragma once

#include "object.h"
#include "raylib.h"
#include "raymath.h"

class Container : public Object
{
public:
	int scroll = 0;
	int scrollMax = 0;
	Container(int x, int y, int width, int height);

	virtual void update(float elapsed);

	virtual void draw();
};