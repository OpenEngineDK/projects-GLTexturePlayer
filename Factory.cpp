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
#include <Resources/FFMPEGResource.h>
#include <Utils/Billboard.h>

#include <string>

using namespace OpenEngine::Logging;
using namespace OpenEngine::Core;
using namespace OpenEngine::Display;
using namespace OpenEngine::Renderers::OpenGL;
using namespace OpenEngine::Scene;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Utils;

string Factory::filename = "";

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
	DirectoryManager::AppendPath("/");
	DirectoryManager::AppendPath(""); //current directory

        // load the resource plug-ins
	ResourceManager<IMovieResource>::AddPlugin(new FFMPEGPlugin());

        engine.AddModule(*(new Statistics(1000)));

	Vector<3,float> pos = Vector<3,float>(0, 10, 40);
	AddMovie(filename, 0.025, pos, root, engine);
    } catch (const Exception& ex) {
        logger.error << "An exception occurred: " << ex.what() << logger.end;
        throw ex;
    }
    // Return true to signal success.
    return true;
}

void Factory::AddMovie(string moviefile, float scale, Vector<3,float> position, ISceneNode* root, IGameEngine& engine) {
	IMovieResourcePtr movie =
	  ResourceManager<IMovieResource>::Create(moviefile);
	engine.AddModule( *(movie.get()) );
        TransformationNode* billboard =
	  Billboard::CreateMovieBillboard(movie, scale);
	billboard->SetPosition(position);
	root->AddNode(billboard);

	MovieKeyHandler* movie_h = new MovieKeyHandler(movie);
	movie_h->RegisterWithEngine(engine);
}
// Get methods for the factory.
IFrame*      Factory::GetFrame()      { return frame; }
IRenderer*   Factory::GetRenderer()   { return renderer; }
