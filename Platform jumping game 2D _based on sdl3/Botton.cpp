#include "Botton.h"
#include "render/Renderer.h"

Botton::Botton() : rect_{0, 0, 0, 0}, color_{100, 100, 100, 255}, text_(""), tsize_(Config::DEFAULT_TEXT_SIZE) {
	clickCallbacks_ = {};
}

Botton::Botton(SDL_FRect rect, std::string text, SDL_Color color, int t_size, std::vector<ClickCallback> callBacks)
	: clickCallbacks_(callBacks), rect_(rect), color_(color), text_(text), tsize_(t_size) {

}

void Botton::clickBottom() noexcept{
	for(const auto& callback : clickCallbacks_) {
		if (callback) {
			callback();
		}
	}

}

void Botton::render() const noexcept {
	Renderer::getInstance().renderText(text_, rect_, color_, tsize_);
}