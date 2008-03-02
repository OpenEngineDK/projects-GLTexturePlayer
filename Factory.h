#ifndef _FACTORY_
#define _FACTORY_

#include <Core/IGameFactory.h>
#include <Display/Camera.h>
#include <Display/Viewport.h>

using namespace OpenEngine::Core;
using namespace OpenEngine::Display;

class Factory : public IGameFactory {
private:
    IFrame*    frame;
    IRenderer* renderer;
    Viewport*  viewport;
    Camera*    camera; 

public:
    Factory();
    bool         SetupEngine(IGameEngine& engine);
    IFrame*      GetFrame();
    IRenderer*   GetRenderer();
};

#endif
