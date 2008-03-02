#include "Factory.h"
#include "MovieKeyHandler.h"

// from OpenEngine
#include <Logging/Logger.h>
#include <Display/Viewport.h>
#include <Display/ViewingVolume.h>
#include <Display/SDLFrame.h>
#include <Devices/SDLInput.h>
#include <Renderers/OpenGL/Renderer.h>
#include <Renderers/OpenGL/RenderingView.h>
#include <Scene/GeometryNode.h>
#include <Scene/TransformationNode.h>
#include <Resources/ResourceManager.h>
#include <Resources/OBJResource.h>
#include <Resources/TGAResource.h>
#include <Utils/Statistics.h>

// from extensions
#include <Utils/MoveHandler.h>
#include <Utils/QuitHandler.h>
#include <Resources/MovieResource.h>
#include <Utils/Billboard.h>

#include <string>

using namespace OpenEngine::Logging;
using namespace OpenEngine::Core;
using namespace OpenEngine::Display;
using namespace OpenEngine::Renderers::OpenGL;
using namespace OpenEngine::Scene;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Utils;

Factory::Factory() {
    frame    = new SDLFrame(800, 600, 32);
    viewport = new Viewport(*frame);
        
    camera = new Camera(*(new ViewingVolume()));
    camera->SetPosition(Vector<3,float>(0,20,80));
    viewport->SetViewingVolume(camera);

    renderer = new Renderer();
    renderer->AddRenderingView(new RenderingView(*viewport));
}

bool Factory::SetupEngine(IGameEngine& engine) {
    try {
        // Setup input handling
        SDLInput* input = new SDLInput();
        engine.AddModule(*input);

        // Register the handler as a listener on up and down keyboard events.
        MoveHandler* move_h = new MoveHandler(*camera);
	move_h->RegisterWithEngine(engine);
        QuitHandler* quit_h = new QuitHandler();
	quit_h->BindToEventSystem();

        // Create scene root
        SceneNode* root = new SceneNode();
        this->renderer->SetSceneRoot(root);

        // Add models from models.txt to the scene
        // First we set the resources directory
	string resourcedir = "projects/GLTexturePlayer/data/";
	ResourceManager::AppendPath(resourcedir);
	logger.info << "Resource directory: " << resourcedir << logger.end;

        // load the resource plug-ins
        ResourceManager::AddModelPlugin(new OBJPlugin());
        ResourceManager::AddTexturePlugin(new TGAPlugin());

        engine.AddModule(*(new Statistics(1000)));

        string moviefilename = resourcedir + "movies/dark.mp4";
        MovieResource* mplayer = new MovieResource(moviefilename);
        engine.AddModule(*mplayer);
        TransformationNode* background = Billboard::CreateMovieBillboard(mplayer, 0.025);
	background->SetPosition(Vector<3,float>(0, 30, 35));
	root->AddNode(background);
  
        string moviefilename2 = resourcedir + "movies/Wrath_Of_The_Lich_King_EN.avi";
        MovieResource* mplayer2 = new MovieResource(moviefilename2);
        engine.AddModule(*mplayer2);
        TransformationNode* background2 = Billboard::CreateMovieBillboard(mplayer2, 0.025);
	background2->SetPosition(Vector<3,float>(45, 5, 35));
	root->AddNode(background2);

        string moviefilename3 = resourcedir + "movies/I_am_Murloc_US.avi";
        MovieResource* mplayer3 = new MovieResource(moviefilename3);
	engine.AddModule(*mplayer3);
        TransformationNode* background3 = Billboard::CreateMovieBillboard(mplayer3, 0.025);
	background3->SetPosition(Vector<3,float>(-45, 5, 35));
	root->AddNode(background3);

        string moviefilename4 = resourcedir + "movies/40_year_old_virgin_med.mov";
        MovieResource* mplayer4 = new MovieResource(moviefilename4);
        engine.AddModule(*mplayer4);
        TransformationNode* background4 = Billboard::CreateMovieBillboard(mplayer4, 0.025);
	background4->SetPosition(Vector<3,float>(0, 5, 35));
	root->AddNode(background4);


        MovieKeyHandler* movie_h = new MovieKeyHandler(mplayer);
	movie_h->RegisterWithEngine(engine);

    } catch (const Exception& ex) {
        logger.error << "An exception occurred: " << ex.what() << logger.end;
        throw ex;
    }
    // Return true to signal success.
    return true;
}

// Get methods for the factory.
IFrame*      Factory::GetFrame()      { return frame; }
IRenderer*   Factory::GetRenderer()   { return renderer; }
