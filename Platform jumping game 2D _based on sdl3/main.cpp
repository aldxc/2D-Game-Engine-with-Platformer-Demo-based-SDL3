#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "Game.h"

int SDL_main(int argc, char* argv[]) {
	Game game;
	game.Run();

	return 0;
}
