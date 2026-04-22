#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <filesystem>
#include "Game.h"


int SDL_main(int argc, char* argv[]) {
	// 创建保存目录
	std::filesystem::create_directories("saves"); 
	Game game;
	game.Run();

	return 0;
}
