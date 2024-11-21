#include "button.h"

extern Vector2 mousePosition;

Button::Button(float x, float y, float width, float height, Color color) : Object(x, y, width, height)
{
	hoverable = true;

	this->color = color;
};

void Button::update(float elapsed)
{
	Object::update(elapsed);

	if (isHovering)
	{
		tint = 0.1f;
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
		{
			clickCallback();
		}
	}
	else
		tint = 0.0f;
};

void Button::draw()
{
	DrawRectangleRec((Rectangle){x, y, width, height}, ColorBrightness(color, tint));
	DrawRectangleLinesEx(Rectangle{float(x), float(y), float(width), float(height)}, 2, GetColor(0x00000033));

	Object::draw();
};