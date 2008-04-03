#ifndef _FACTORY_
#define _FACTORY_

#include <Core/IGameFactory.h>
#include <Display/Camera.h>
#include <Display/Viewport.h>

class OpenEngine::Scene::ISceneNode;

using OpenEngine::Scene::ISceneNode;

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
    void AddMovie(string filename, float scale, Vector<3,float> position, ISceneNode* root, IGameEngine& engine);
    IFrame*      GetFrame();
    IRenderer*   GetRenderer();

    static string filename;
};

#endif
