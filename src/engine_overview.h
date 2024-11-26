#include "global.h"
#include "engine.h"
#include "ui/button.h"
#include "ui/container.h"
#include "ui/sprite.h"
#include "ui/text.h"

class ModContainer : public Container
{
public:
	Text *name;
	Text *version;
	Button *openFolder;
	ModContainer(std::string name, std::string version, std::string path);
};

class ModsContainer : public Container
{
public:
	ModsContainer();

	void update(float elapsed);
	void draw();
};

class EngineOverview : public Container
{
public:
	Sprite *engineIcon;
	Text *engineName;
	Text *description;
	Button *playButton;

	Engine *engine;

	EngineOverview(Engine *engine);

	void changeEngine(Engine newEngine);

	void confirmEngineClose();
};