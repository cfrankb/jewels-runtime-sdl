#include "game.h"
#include <SDL2/SDL.h>

class CRuntime : public CGame
{
public:
    CRuntime();
    virtual ~CRuntime();

    void paint();
    void run();
    bool SDLInit();
    void doInput();

protected:
    static void cleanup();
    enum
    {
        AIM_UP = 0,
        AIM_DOWN = 1,
        AIM_LEFT = 2,
        AIM_RIGHT = 3,

        MODE_START_GAME = 0,
        MODE_NEW_SHAPE = 1,
    };

    typedef struct
    {
        SDL_Renderer *renderer;
        SDL_Window *window;
        SDL_Texture *texture;
    } App;

    App m_app;

    int m_mode = 0;
};