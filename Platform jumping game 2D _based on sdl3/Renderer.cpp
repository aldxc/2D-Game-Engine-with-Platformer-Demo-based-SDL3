#include "Renderer.h"
#include "Config.h"

Renderer::Renderer() {
	if (!init()) {
		SDL_Log("Failed to initialize renderer.");
		return;
	}

}

Renderer::~Renderer(){
	//text_.reset();
	//textEngine_.reset();
	//font_.reset();
	renderer_.reset();
	window_.reset();

	TTF_Quit();
	SDL_Quit();
}

bool Renderer::init() noexcept{
	SDL_Window* window = SDL_CreateWindow("Platform Jumping Game 2D", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	if (!window) {
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return;
	}
	window_.reset(window);

	SDL_Renderer* renderer = SDL_CreateRenderer(window_.get(), "render");
	if (!renderer) {
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return;
	}
	renderer_.reset(renderer);
}

void Renderer::beginRender() const noexcept{
	SDL_SetRenderDrawColor(renderer_.get(), 0, 0, 0, 255);//重置颜色
	SDL_RenderClear(renderer_.get());
}

void Renderer::restoreDefaultAndPresent() const noexcept {
	//重置字体
	SDL_RenderPresent(renderer_.get());//更新屏幕
}

