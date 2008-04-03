#ifndef _MOVIE_KEY_HANDLER_
#define _MOVIE_KEY_HANDLER_

// from core
#include <Devices/IKeyboard.h>
#include <Devices/Symbols.h>
#include <Resources/IMovieResource.h>

using namespace OpenEngine::Devices;

class MovieKeyHandler {
private:
    IMovieResourcePtr movie;
    bool pause;
public:
    MovieKeyHandler(IMovieResourcePtr movie) : movie(movie), pause(false) {}
    void HandleKey(KeyboardEventArg arg) {
        if (arg.sym == KEY_SPACE) {
            pause = !pause;
            if(movie!=NULL)
                movie->Pause(pause);
        }
    }

    void RegisterWithEngine(IGameEngine& engine) {
        Listener<MovieKeyHandler,KeyboardEventArg>* downlist =
            new Listener<MovieKeyHandler,KeyboardEventArg>
	  (*this,&MovieKeyHandler::HandleKey);
        IKeyboard::keyDownEvent.Add(downlist);
    }
};

#endif
