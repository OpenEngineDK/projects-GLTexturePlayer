// OpenEngine GLTexturePlayer demo.
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

// OpenEngine stuff
#include <Meta/Config.h>

// Core structures
#include <Core/Engine.h>
#include <Core/EngineEvents.h>

// Display structures
#include <Display/IFrame.h>
#include <Display/Camera.h>
#include <Display/ViewingVolume.h>
// SDL implementation
#include <Display/SDLFrame.h>
#include <Devices/SDLInput.h>

// OpenGL rendering implementation
#include <Renderers/OpenGL/Renderer.h>
#include <Renderers/OpenGL/RenderingView.h>

// Resources
#include <Resources/DirectoryManager.h>
#include <Resources/ResourceManager.h>
// Resource plugins
#include <Resources/ITextureResource.h>
#include <Resources/FFMPEGResource.h>
#include <Resources/TGAResource.h>
#include <Resources/ScaledTextureResource.h>

#include <Renderers/TextureLoader.h>

// Scene structures
#include <Scene/ISceneNode.h>
#include <Scene/SceneNode.h>
#include <Scene/GeometryNode.h>
#include <Scene/TransformationNode.h>

// Utilities and logger
#include <Logging/Logger.h>
#include <Logging/StreamLogger.h>
#include <Utils/Statistics.h> 

// OERacer utility files
#include <Utils/MoveHandler.h>
#include <Utils/QuitHandler.h>

// project files
#include <Utils/Billboard.h>
#include "MovieKeyHandler.h"

// Additional namespaces
using namespace OpenEngine;
using namespace OpenEngine::Core;
using namespace OpenEngine::Logging;
using namespace OpenEngine::Devices;
using namespace OpenEngine::Display;
using namespace OpenEngine::Renderers::OpenGL;
using namespace OpenEngine::Renderers;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Utils;

// Configuration structure to pass around to the setup methods
struct Config {
    IEngine&              engine;
    IFrame*               frame;
    Viewport*             viewport;
    IViewingVolume*       viewingvolume;
    Camera*               camera;
    IRenderer*            renderer;
    IMouse*               mouse;
    IKeyboard*            keyboard;
    ISceneNode*           scene;
    bool                  resourcesLoaded;
    string                filename;
    Renderers::TextureLoader*        textureLoader;
    Config(IEngine& engine)
        : engine(engine)
        , frame(NULL)
        , viewport(NULL)
        , viewingvolume(NULL)
        , camera(NULL)
        , renderer(NULL)
        , mouse(NULL)
        , keyboard(NULL)
        , scene(NULL)
        , resourcesLoaded(false)
        , filename("")
        , textureLoader(NULL)
    {}
};

// Forward declaration of the setup methods
void SetupResources(Config&);
void SetupDevices(Config&);
void SetupDisplay(Config&);
void SetupRendering(Config&);
void SetupScene(Config&);
void SetupDebugging(Config&);

int main( int argc, char** argv ) {
    // Setup logging facilities.
    Logger::AddLogger(new StreamLogger(&std::cout));

    if (argc==2) {
      Engine* engine = new Engine();
      Config config(*engine);
      string filename = config.filename = argv[1];

      logger.info << "playing file: " << filename << logger.end;
      logger.info << "=====================================================" << logger.end;
      logger.info << "Keyboard controls:"                                             << logger.end;
      logger.info << " Space : Pause/resume a playing movie."                         << logger.end;
      logger.info << "     w : Move positive direction on the z-axis."         << logger.end;
      logger.info << "     s : Move negative direction on the z-axis."         << logger.end;
      logger.info << "     d : Move positive direction on the x-axis."         << logger.end;
      logger.info << "     a : Move negative direction on the x-axis."         << logger.end;
      logger.info << "Moving the mouse up and down triggers pitch."          << logger.end;
      logger.info << "Moving the mouse right and left triggers yaw."         << logger.end;
      logger.info << "=====================================================" << logger.end;
    
      // Setup the engine
      SetupResources(config);
      SetupDisplay(config);
      SetupDevices(config);
      SetupRendering(config);
      SetupScene(config);

      // Possibly add some debugging stuff
      SetupDebugging(config);

      // Start up the engine.
      engine->Start();

      // Return when the engine stops.
      delete engine;
      //@todo: delete config;

    } else {
      logger.info << "wrong number of parameters - first parameter must be a filename" << logger.end;
    }
    // Return when the engine stops.
    return EXIT_SUCCESS;
}

void SetupResources(Config& config) {
    // current directory
	DirectoryManager::AppendPath("/");
	DirectoryManager::AppendPath("");
    
    // load the resource plug-ins
	ResourceManager<IMovieResource>::AddPlugin(new FFMPEGPlugin());
	ResourceManager<ITextureResource>::AddPlugin(new TGAPlugin());

    config.resourcesLoaded = true;
}

void SetupDisplay(Config& config) {
    if (config.frame         != NULL ||
        config.viewingvolume != NULL ||
        config.camera        != NULL ||
        config.viewport      != NULL)
        throw Exception("Setup display dependencies are not satisfied.");

    SDLFrame* sdlframe = new SDLFrame(800, 600, 32);
    config.frame         = sdlframe;
    config.viewingvolume = new ViewingVolume();
    config.camera        = new Camera( *config.viewingvolume );
    config.viewport      = new Viewport(*config.frame);
    config.viewport->SetViewingVolume(config.camera);

    sdlframe->Handle(InitializeEventArg());
    //config.engine.InitializeEvent().Attach(*config.frame);
    config.engine.ProcessEvent().Attach(*config.frame);
    config.engine.DeinitializeEvent().Attach(*config.frame);
}

void SetupDevices(Config& config) {
    if (config.keyboard != NULL ||
        config.mouse    != NULL)
        throw Exception("Setup devices dependencies are not satisfied.");
    // Create the mouse and keyboard input modules
    SDLInput* input = new SDLInput();
    config.keyboard = input;
    config.mouse = input;

    // Bind the quit handler
    QuitHandler* quit_h = new QuitHandler(config.engine);
    config.keyboard->KeyEvent().Attach(*quit_h);

    // Register the handler as a listener on up and down keyboard events.
    MoveHandler* move_h = new MoveHandler(*config.camera, *config.mouse);
    config.keyboard->KeyEvent().Attach(*move_h);
    config.engine.InitializeEvent().Attach(*move_h);
    config.engine.ProcessEvent().Attach(*move_h);
    config.engine.DeinitializeEvent().Attach(*move_h);

    // Bind to the engine for processing time
    config.engine.InitializeEvent().Attach(*input);
    config.engine.ProcessEvent().Attach(*input);
    config.engine.DeinitializeEvent().Attach(*input);
}

void SetupRendering(Config& config) {
    if (config.viewport == NULL ||
        config.renderer != NULL ||
        config.camera == NULL )
        throw Exception("Setup renderer dependencies are not satisfied.");

    // Create a renderer
    config.renderer = new Renderer(config.viewport);

    // Setup a rendering view
    RenderingView* rv = new RenderingView(*config.viewport);
    config.renderer->ProcessEvent().Attach(*rv);

    // Add rendering initialization tasks
    config.textureLoader = new Renderers::TextureLoader(*config.renderer);

    config.engine.InitializeEvent().Attach(*config.renderer);
    config.engine.ProcessEvent().Attach(*config.renderer);
    config.engine.DeinitializeEvent().Attach(*config.renderer);
}

void AddMovie(string moviefile, float scale, Vector<3,float> position,
              ISceneNode* root, Config& config) {
	IMovieResourcePtr movie =
        //ResourceManager<IMovieResource>::Create(moviefile);
        FFMPEGResource::Create(moviefile, false);

    config.engine.InitializeEvent().Attach(*(movie.get()));
    config.engine.ProcessEvent().Attach(*(movie.get()));
    config.engine.DeinitializeEvent().Attach(*(movie.get()));

    config.textureLoader->
        Load( movie, Renderers::TextureLoader::RELOAD_IMMEDIATE );

    TransformationNode* billboard =
	  Billboard::CreateMovieBillboard(movie, scale);
	billboard->SetPosition(position);
	root->AddNode(billboard);

	MovieKeyHandler* movie_h = new MovieKeyHandler(movie);
    config.keyboard->KeyEvent().Attach(*movie_h);
}

void AddTexture(ITextureResourcePtr texture,
                float scale, Vector<3,float> position,
                ISceneNode* root, Config& config) {
    TransformationNode* billboard =
	  Billboard::CreateTextureBillboard(texture, scale);
	billboard->SetPosition(position);
	root->AddNode(billboard);
}

void SetupScene(Config& config) {
    if (config.scene  != NULL ||
        config.mouse  == NULL ||
        config.keyboard == NULL ||
        config.resourcesLoaded == false)
        throw Exception("Setup scene dependencies are not satisfied.");

        // Create scene root
        SceneNode* root = new SceneNode();
        config.scene = root;

        Vector<3,float> pos = Vector<3,float>(10, 0, 0);

        const bool loadMovie = true;
        if (!loadMovie) {
            ITextureResourcePtr tex =
                ResourceManager<ITextureResource>::Create(config.filename);
            config.textureLoader->Load(tex);
            //tex->Load();
            AddTexture(tex, 0.025, pos, root, config);
            
            ITextureResourcePtr scaledTex = ITextureResourcePtr
                ( new ScaledTextureResource(tex, 2) ); 
            config.textureLoader->Load(scaledTex);
            //scaledTex->Load();
            AddTexture(scaledTex, 0.025*2, -pos, root, config);
        }
        else
            AddMovie(config.filename, 0.025, pos, root, config);

    // Supply the scene to the renderer
    config.renderer->SetSceneRoot(config.scene);
}

void SetupDebugging(Config& config) {
    // Add Statistics module
    config.engine.ProcessEvent()
        .Attach(*(new OpenEngine::Utils::Statistics(1000)));
}
