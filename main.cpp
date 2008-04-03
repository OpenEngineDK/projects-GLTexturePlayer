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

#include "Factory.h"

#include <Core/GameEngine.h>
#include <Logging/Logger.h>
#include <Logging/StreamLogger.h>

using OpenEngine::Core::GameEngine;
using namespace OpenEngine::Logging;

int main( int argc, char** argv ) {
    // Setup logging facilities.
    Logger::AddLogger(new StreamLogger(&std::cout));

    if (argc==2) {
      string filename = Factory::filename = argv[1];
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
    
    // Start the engine.
    IGameEngine& engine = GameEngine::Instance();
    engine.Start(new Factory());
    } else {
    logger.info << "wrong number of parameters - first parameter must be a filename" << logger.end;
    }
    // Return when the engine stops.
    return EXIT_SUCCESS;
}
