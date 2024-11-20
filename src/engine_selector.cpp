#include "engine_selector.h"

// EngineContainer
#pragma region EngineContainer
EngineContainer::EngineContainer(const char *enginename, Texture2D engineicon, const char *engineversion, const char *folderpath) : Container(0, 0, 400, 75)
{
    name = enginename;
    path = folderpath;
    version = engineversion;

    spriteicon = new Sprite(15, 7, engineicon);
    add(spriteicon);
    textname = new Text(90, 17, 250, 25, enginename, 24.0f, WHITE, DEFAULTFONTBOLD);
    add(textname);
    textversion = new Text(90, 40, 250, 19, engineversion, 18.0f, {255, 255, 255, 178}, DEFAULTFONT);
    add(textversion);
}

void EngineContainer::draw()
{
    DrawRectangle(x, y, width, height, BACKGROUNDSECONDARYCOLOR);
    DrawRectangleLinesEx({float(x), float(y), float(width), float(height)}, 2.0f, GetColor(0x00000033));

    Container::draw();
}
#pragma endregion

// EngineSelector
#pragma region EngineSelector
EngineSelector::EngineSelector() : Container(50, 50, 400, 620)
{
    EngineContainer *codename = new EngineContainer("Codename Engine", LoadTexture(ASSETS_PATH"codename.png"), "1.0.0", "");
    add(codename);
    EngineContainer *codename2 = new EngineContainer("Codename Engine Dev", LoadTexture(ASSETS_PATH"codename.png"), "dis shit ain't even releasd", "");
    add(codename2);
    EngineContainer *psych = new EngineContainer("Psych Engine", LoadTexture(ASSETS_PATH"psych.png"), "0.7.3", "");
    add(psych);
    EngineContainer *fpsplus = new EngineContainer("FPS Plus Engine", LoadTexture(ASSETS_PATH"fpsplus.png"), "6.0.1", "");
    add(fpsplus);
    EngineContainer *os = new EngineContainer("OS Engine", LoadTexture(ASSETS_PATH"unknown.png"), "2 trash 2 actually have a version", "");
    add(os);
}

void EngineSelector::update(float elapsed) {
    int addY = 0;
    for (auto child : children) {
        child->y = addY;
        addY += child->height;
    }

    Container::update(elapsed);
}

void EngineSelector::draw()
{
    DrawRectangle(x, y, width, height, LISTCOLOR);
    DrawRectangleLinesEx({float(x), float(y), float(width), float(height)}, 2.0f, GetColor(0x00000033));

    Container::draw();
}
#pragma endregion