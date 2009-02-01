#ifndef _MOVIE_KEY_HANDLER_
#define _MOVIE_KEY_HANDLER_

// from core
#include <Devices/IKeyboard.h>
#include <Devices/Symbols.h>
#include <Resources/IMovieResource.h>

using namespace OpenEngine::Devices;

class MovieKeyHandler : public IListener<KeyboardEventArg> {
private:
    IMovieResourcePtr movie;
    bool pause;
public:
    MovieKeyHandler(IMovieResourcePtr movie) : movie(movie), pause(false) {}
    void Handle(KeyboardEventArg arg) {
        if (arg.type == EVENT_PRESS
	    && arg.sym == KEY_SPACE) {
            pause = !pause;
            if(movie!=NULL)
                movie->Pause(pause);
        }
    }
};

#endif
