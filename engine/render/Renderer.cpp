#include "Renderer.h"

Renderer::Renderer() {

}

Renderer::~Renderer(){
	text_.reset();
	textEngine_.reset();
	font_.reset();
	renderer_.reset();
	window_.reset();

	TTF_Quit();
	SDL_Quit();
}

bool Renderer::init(int w, int h) noexcept{
	if (!TTF_Init()) {
		SDL_Log("Failed to init TTF: %s", SDL_GetError());
		return false;
	}

	SDL_Window* window = SDL_CreateWindow("Platform Jumping Game 2D", w, h, 0);
	if (!window) {
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	window_.reset(window);

	SDL_Renderer* renderer = SDL_CreateRenderer(window_.get(), nullptr);
	if (!renderer) {
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}
	renderer_.reset(renderer);

	TTF_TextEngine* textEngine = TTF_CreateRendererTextEngine(renderer_.get());
	if (!textEngine) {
		SDL_Log("Failed to create TextEngine: %s", SDL_GetError());
		return false;
	}
	textEngine_.reset(textEngine);

	std::string path = "font.ttf";
	TTF_Font* font = TTF_OpenFont(path.c_str(), 30);
	if (!font) {
		SDL_Log("Failed to open font: %s", SDL_GetError());
		return false;
	}
	font_.reset(font);

	SDL_Texture* staticTexture = SDL_CreateTexture(renderer_.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
	if(!staticTexture) {
		SDL_Log("Failed to create static texture: %s", SDL_GetError());
		return false;
	}
	staticTexture_.reset(staticTexture);

	SDL_Texture* dynamicTexture = SDL_CreateTexture(renderer_.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
	if(!dynamicTexture) {
		SDL_Log("Failed to create dynamic texture: %s", SDL_GetError());
		return false;
	}
	dynamicTexture_.reset(dynamicTexture);

	return true;
}

void Renderer::beginRender() const noexcept{
	SDL_SetRenderDrawColor(renderer_.get(), 0, 0, 0, 255);//重置颜色
	SDL_RenderClear(renderer_.get());
}

void Renderer::restoreDefaultAndPresent() const noexcept {
	//重置字体
	TTF_SetFontSize(font_.get(), 30);
	SDL_RenderPresent(renderer_.get());//更新屏幕
}

void Renderer::renderRect(const SDL_FRect& rect, const SDL_Color& color) const noexcept{
	SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
	SDL_RenderRect(renderer_.get(), &rect);
}

void Renderer::renderFillRect(const SDL_FRect& rect, const SDL_Color& color) const noexcept{
	SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer_.get(), &rect);
}

void Renderer::renderText(const std::string text, const SDL_FRect& rect, const SDL_Color color, const int t_size) const noexcept{
	TTF_SetFontSize(font_.get(), t_size);

	TTF_Text* t = TTF_CreateText(textEngine_.get(), font_.get(), text.c_str(), text.size());
	if (!t) {
		SDL_Log("Fail to create text : %s", SDL_GetError());
		return;
	}
	text_.reset(t);
	TTF_SetTextColor(t, color.r, color.g, color.b, color.a);
	int w = 0, h = 0;
	TTF_GetTextSize(text_.get(), &w, &h);
	//居中绘制
	float x = rect.x + (rect.w - w) / 2.0f;
	float y = rect.y + (rect.h - h) / 2.0f;
	TTF_DrawRendererText(t, x, y);
}

void Renderer::resetRenderTarget() noexcept{
	SDL_SetRenderTarget(renderer_.get(), nullptr);
}

void Renderer::clearStaticTexture(const SDL_Color color) noexcept{
	SDL_SetRenderTarget(renderer_.get(), staticTexture_.get());
	SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
	SDL_RenderClear(renderer_.get());
}

void Renderer::clearDynamicTexture(const SDL_Color color) noexcept{
	SDL_SetRenderTarget(renderer_.get(), dynamicTexture_.get());
	SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
	SDL_RenderClear(renderer_.get());
}

void Renderer::renderStaticTexture() const noexcept{
	SDL_RenderTexture(renderer_.get(), staticTexture_.get(), nullptr, nullptr);
}

void Renderer::renderDynamicTexture() const noexcept{
	SDL_SetRenderDrawBlendMode(renderer_.get(), SDL_BLENDMODE_BLEND); // 开启混合模式以支持动态纹理的透明度
	SDL_RenderTexture(renderer_.get(), dynamicTexture_.get(), nullptr, nullptr);
}

