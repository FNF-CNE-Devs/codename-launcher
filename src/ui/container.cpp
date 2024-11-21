#include "container.h"

Container::Container(float x, float y, float width, float height) : Object(x, y, width, height)
{
	hoverable = true;
}

void Container::update(float elapsed)
{
	Vector2 mousePosition = GetMousePosition();

	isHovering = mousePosition.x == Clamp(mousePosition.x, x, x + width) && mousePosition.y == Clamp(mousePosition.y, y, y + height);

	Object *lastChild = children.back();
	scrollMax = std::max(0.0f, (lastChild->y + lastChild->height) - height);
	if (isHovering)
		scroll = (int)Clamp(scroll - (int)(GetMouseWheelMove() * 20), 0, scrollMax);
	// TraceLog(LOG_INFO, TextFormat("%i", scroll));

	y -= scroll;
	Object::update(elapsed);
	y += scroll;
}

void Container::draw()
{
	BeginScissorMode(x, y, width, height);

	y -= scroll;
	Object::draw();
	y += scroll;

	EndScissorMode();

	if (scrollMax != 0)
	{
		DrawRectangleRounded(Rectangle{float(x + width - 5), float(y + scroll), 4, float(height - scrollMax)}, 1.0f, 10, WHITE);
	}
}