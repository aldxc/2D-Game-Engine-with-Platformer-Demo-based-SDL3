#include "Botton.h"
#include "Renderer.h"

Botton::Botton() : rect_{0, 0, 0, 0}, color_{100, 100, 100, 255}, text_(""), tsize_(Config::DEFAULT_TEXT_SIZE) {
	clickCallback_ = nullptr;
}

Botton::Botton(SDL_FRect rect, std::string text, SDL_Color color, int t_size, ClickCallback callBack)
	: clickCallback_(callBack), rect_(rect), color_(color), text_(text), tsize_(t_size) {

}

void Botton::clickBottom() noexcept{
	if (clickCallback_) {
		clickCallback_();
	}

}

void Botton::render() const noexcept {
	Renderer::getInstance().renderText(text_, rect_, color_, tsize_);
}