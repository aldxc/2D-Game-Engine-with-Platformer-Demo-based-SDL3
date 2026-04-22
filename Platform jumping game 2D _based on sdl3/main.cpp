#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <filesystem>
#include "Game.h"


int SDL_main(int argc, char* argv[]) {
	std::filesystem::create_directories("saves"); // 创建保存目录，后续增加存档管理等功能)
	Game game;
	game.Run();

	return 0;
}
