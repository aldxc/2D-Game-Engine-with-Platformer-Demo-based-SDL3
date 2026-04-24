#include "Botton.h"
#include "render/Renderer.h"

Botton::Botton() : m_rect{0, 0, 0, 0}, m_color{100, 100, 100, 255}, m_text(""), m_tsize(Config::DEFAULT_TEXT_SIZE) {
	m_clickCallbacks = {};
}

Botton::Botton(SDL_FRect rect, std::string text, SDL_Color color, int t_size, std::vector<ClickCallback> callBacks)
	: m_clickCallbacks(callBacks), m_rect(rect), m_color(color), m_text(text), m_tsize(t_size) {

}

void Botton::clickBottom() const noexcept {
	for(const auto& callback : m_clickCallbacks) {
		if (callback) {
			callback();
		}
	}

}

void Botton::render(Renderer& renderer) const noexcept {
	renderer.renderText(m_text, m_rect, m_color, m_tsize);
}