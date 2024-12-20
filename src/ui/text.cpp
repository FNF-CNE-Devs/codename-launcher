#include "text.h"

Text::Text(float x, float y, std::string text, float width, float height, float size, bool wrap, Color color, std::string font)
    : Object(x, y, width, height)
{
	this->text = text;
	this->size = size;
	this->color = color;
	this->font = LoadFontEx(font.c_str(), int(size), NULL, 0);
	this->wrap = wrap;

	Vector2 sizeMeasurement = MeasureTextEx(this->font, text.c_str(), this->size, 0.5f);
	autoWidth = (this->width == 0);
	autoHeight = (this->height == 0);
	if (autoWidth)
		this->width = sizeMeasurement.x;
	if (autoHeight)
		this->height = sizeMeasurement.y;
	SetTextureFilter(this->font.texture, TEXTURE_FILTER_BILINEAR);

	SetTextLineSpacing(0);
};

void Text::draw()
{
	Object::draw();

	DrawTextBoxedSelectable(
	    font, text + " ", Rectangle{float(x), float(y), float(width) + 1, float(height)}, size, 0.5f, wrap, color, BLACK, BLUE
	);
};

void Text::setText(std::string text)
{
	this->text = text;
	Vector2 sizeMeasurement = MeasureTextEx(this->font, this->text.c_str(), this->size, 0.5f);
	if (autoWidth)
		this->width = sizeMeasurement.x;
	if (autoHeight)
		this->height = sizeMeasurement.y;
}

void Text::DrawTextBoxedSelectable(
    Font font,
    std::string text,
    Rectangle rec,
    float fontSize,
    float spacing,
    bool wordWrap,
    Color tint,
    Color selectTint,
    Color selectBackTint
)
{
	int selectStart = 0;
    int selectLength = 0;
	unsigned int length = TextLength(text.c_str()); // Total length in bytes of the text, scanned by codepoints in loop

	float textOffsetY = 0;	  // Offset between lines (on line break '\n')
	float textOffsetX = 0.0f; // Offset X to next character to draw

	float scaleFactor = fontSize / (float)font.baseSize; // Character rectangle scaling factor

	// Word/character wrapping mechanism variables
	enum
	{
		MEASURE_STATE = 0,
		DRAW_STATE = 1
	};
	int state = wordWrap ? MEASURE_STATE : DRAW_STATE;

	int startLine = -1; // Index where to begin drawing (where a line begins)
	int endLine = -1;   // Index where to stop drawing (where a line ends)
	int lastk = -1;	    // Holds last value of the character position

	for (int i = 0, k = 0; i < (int)length; i++, k++)
	{
		// Get next codepoint from byte string and glyph index in font
		int codepointByteCount = 0;
		int codepoint = GetCodepoint(&text[(unsigned int)i], &codepointByteCount);
		int index = GetGlyphIndex(font, codepoint);

		// NOTE: Normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
		// but we need to draw all of the bad bytes using the '?' symbol moving one byte
		if (codepoint == 0x3f)
			codepointByteCount = 1;
		i += (codepointByteCount - 1);

		float glyphWidth = 0;
		if (codepoint != '\n')
		{
			glyphWidth = (font.glyphs[index].advanceX == 0) ? font.recs[index].width * scaleFactor
									: font.glyphs[index].advanceX * scaleFactor;

			if (i + 1 < length)
				glyphWidth = glyphWidth + spacing;
		}

		// NOTE: When wordWrap is ON we first measure how much of the text we can draw before going outside of the rec container
		// We store this info in startLine and endLine, then we change states, draw the text between those two variables
		// and change states again and again recursively until the end of the text (or until we get outside of the container).
		// When wordWrap is OFF we don't need the measure state so we go to the drawing state immediately
		// and begin drawing on the next line before we can get outside the container.
		if (state == MEASURE_STATE)
		{
			// TODO: There are multiple types of spaces in UNICODE, maybe it's a good idea to add support for more
			// Ref: http://jkorpela.fi/chars/spaces.html
			if ((codepoint == ' ') || (codepoint == '\t') || (codepoint == '\n'))
				endLine = i;

			if ((textOffsetX + glyphWidth) > rec.width)
			{
				endLine = (endLine < 1) ? i : endLine;
				if (i == endLine)
					endLine -= codepointByteCount;
				if ((startLine + codepointByteCount) == endLine)
					endLine = (i - codepointByteCount);

				state = !state;
			}
			else if ((i + 1) == length)
			{
				endLine = i;
				state = !state;
			}
			else if (codepoint == '\n')
				state = !state;

			if (state == DRAW_STATE)
			{
				textOffsetX = 0;
				i = startLine;
				glyphWidth = 0;

				// Save character position when we switch states
				int tmp = lastk;
				lastk = k - 1;
				k = tmp;
			}
		}
		else
		{
			if (codepoint == '\n')
			{
				if (!wordWrap)
				{
					textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor;
					textOffsetX = 0;
				}
			}
			else
			{
				if (!wordWrap && ((textOffsetX + glyphWidth) > rec.width))
				{
					textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor;
					textOffsetX = 0;
				}

				// When text overflows rectangle height limit, just stop drawing
				if ((textOffsetY + font.baseSize * scaleFactor) > rec.height)
					break;

				Vector2 mousePosition = GetMousePosition();
				if (mousePosition.y == Clamp(mousePosition.y, rec.y + textOffsetY, rec.y + textOffsetY + ((float)font.baseSize * scaleFactor))) {
					if (IsMouseButtonPressed(0)) {
						if (mousePosition.x == Clamp(mousePosition.x, rec.x + textOffsetX - (glyphWidth / 2), rec.x + textOffsetX + (glyphWidth / 2)))
							this->selectStart = i;
					}
					if (IsMouseButtonDown(0) && this->selectStart != -1) {
						if (mousePosition.x == Clamp(mousePosition.x, rec.x + textOffsetX - (glyphWidth / 3), rec.x + textOffsetX + (glyphWidth / 3)))
							selectEnd = i;
					}
				}
				else if (IsMouseButtonPressed(0)) {
					this->selectStart = -1;
					selectEnd = -1;
				}
				selectStart = std::max(std::min(this->selectStart, selectEnd), 0);
				selectLength = std::abs(this->selectStart - selectEnd);

				// Draw selection background
				bool isGlyphSelected = false;
				if ((selectStart >= 0) && (k >= selectStart) && (k < (selectStart + selectLength)))
				{
					DrawRectangleRec(
					    Rectangle{
						rec.x + textOffsetX - 1,
						rec.y + textOffsetY,
						glyphWidth,
						(float)font.baseSize * scaleFactor},
					    selectBackTint
					);
					isGlyphSelected = true;
				}
				// Draw current character glyph
				if ((codepoint != ' ') && (codepoint != '\t'))
				{
					DrawTextCodepoint(
					    font,
					    codepoint,
					    Vector2{rec.x + textOffsetX, rec.y + textOffsetY},
					    fontSize,
					    isGlyphSelected ? selectTint : tint
					);
				}
			}

			if (wordWrap && (i == endLine))
			{
				textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor / 1.75;
				textOffsetX = 0;
				startLine = endLine;
				endLine = -1;
				glyphWidth = 0;
				selectStart += lastk - k;
				k = lastk;

				state = !state;
			}
		}

		if ((textOffsetX != 0) || (codepoint != ' '))
			textOffsetX += glyphWidth; // avoid leading spaces
	}
}