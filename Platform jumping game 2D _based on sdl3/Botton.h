#pragma once
#include <string>
#include <functional>
#include <SDL3/SDL.h>
#include <vector>
#include "core/State.h"
#include "Config.h"

class Renderer;

class Botton {
public:
	using ClickCallback = std::function<void()>;
	explicit Botton();
	Botton(SDL_FRect rect, std::string text, SDL_Color color = SDL_Color({ 100, 100, 100, 255 }), int t_size = Config::DEFAULT_TEXT_SIZE, std::vector<ClickCallback> callBacks = {});
	~Botton() = default;

	void clickBottom() const noexcept;
	void render(Renderer& renderer) const noexcept;
	const SDL_FRect& getRect() const noexcept { return rect_; }
private:
	SDL_FRect rect_;
    SDL_Color color_;
	std::string text_;
	int tsize_;
	std::vector<ClickCallback> clickCallbacks_;
};