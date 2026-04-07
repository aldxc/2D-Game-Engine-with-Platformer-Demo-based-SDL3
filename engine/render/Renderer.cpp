#include "Renderer.h"

Renderer::Renderer() {

}

Renderer::~Renderer(){
	textCache_.clear();
	textEngine_.reset();
	font_.reset();
	renderer_.reset();
	window_.reset();

	TTF_Quit();
	SDL_Quit();
}

bool Renderer::init(int w, int h, int logicW, int logicH, int fontSize) noexcept{
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
	SDL_SetRenderTarget(renderer_.get(), nullptr);
	SDL_SetRenderLogicalPresentation(renderer_.get(), logicW, logicH, SDL_LOGICAL_PRESENTATION_LETTERBOX); // 后续根据返回值添加判断，后续抽象为成员方法

	TTF_TextEngine* textEngine = TTF_CreateRendererTextEngine(renderer_.get());
	if (!textEngine) {
		SDL_Log("Failed to create TextEngine: %s", SDL_GetError());
		return false;
	}
	textEngine_.reset(textEngine);

	std::string path = "font.ttf";
	TTF_Font* font = TTF_OpenFont(path.c_str(), fontSize);
	if (!font) {
		SDL_Log("Failed to open font: %s", SDL_GetError());
		return false;
	}
	font_.reset(font);

	SDL_Texture* UITexture = SDL_CreateTexture(renderer_.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, logicW, logicH);
	if(!UITexture) {
		SDL_Log("Failed to create UI texture: %s", SDL_GetError());
		return false;
	}
	UITexture_.reset(UITexture);

	SDL_Texture* staticTexture = SDL_CreateTexture(renderer_.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, logicW, logicH);
	if(!staticTexture) {
		SDL_Log("Failed to create static texture: %s", SDL_GetError());
		return false;
	}
	staticTexture_.reset(staticTexture);

	SDL_Texture* dynamicTexture = SDL_CreateTexture(renderer_.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, logicW, logicH);
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
	SDL_SetRenderTarget(renderer_.get(), nullptr);

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
	std::unique_ptr<TTF_Text, TTF_TEXTDeleter> text_;

	auto it = textCache_.find(text);
	if(it == textCache_.end()) {
		// 没有找到缓存，创建新的文本对象
		TTF_Text* t = TTF_CreateText(textEngine_.get(), font_.get(), text.c_str(), text.size());
		if (!t) {
			SDL_Log("Fail to create text : %s", SDL_GetError());
			return;
		}
		text_.reset(t);
	} else {
		text_ = std::move(it->second);
	}
	
	TTF_SetTextColor(text_.get(), color.r, color.g, color.b, color.a);
	int w = 0, h = 0;
	TTF_GetTextSize(text_.get(), &w, &h);
	//居中绘制
	float x = rect.x + (rect.w - w) / 2.0f;
	float y = rect.y + (rect.h - h) / 2.0f;
	TTF_DrawRendererText(text_.get(), x, y);
	textCache_[text] = std::move(text_);
}

void Renderer::renderTexture(SDL_Texture* texture, const SDL_FRect srcrect, const SDL_FRect dstrect) const noexcept {
	SDL_RenderTexture(renderer_.get(), texture, &srcrect, &dstrect);
}

void Renderer::resetRenderTarget() noexcept{
	SDL_SetRenderTarget(renderer_.get(), nullptr);
	SDL_SetRenderLogicalPresentation(renderer_.get(), 1024, 576, SDL_LOGICAL_PRESENTATION_LETTERBOX);
}

void Renderer::clearUITexture(const SDL_Color color) noexcept{
	SDL_SetRenderTarget(renderer_.get(), UITexture_.get());

	SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
	SDL_RenderClear(renderer_.get());
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
	SDL_SetRenderDrawBlendMode(renderer_.get(), SDL_BLENDMODE_BLEND); // 开启混合模式以支持动态纹理的透明度
	SDL_RenderTexture(renderer_.get(), staticTexture_.get(), nullptr, nullptr);

}

void Renderer::renderDynamicTexture() const noexcept{
	//SDL_SetRenderDrawBlendMode(renderer_.get(), SDL_BLENDMODE_BLEND); // 开启混合模式以支持动态纹理的透明度
	SDL_RenderTexture(renderer_.get(), dynamicTexture_.get(), nullptr, nullptr);
}

void Renderer::renderUITexture() const noexcept{
	SDL_SetRenderDrawBlendMode(renderer_.get(), SDL_BLENDMODE_BLEND); // 开启混合模式以支持UI纹理的透明度
	SDL_RenderTexture(renderer_.get(), UITexture_.get(), nullptr, nullptr);
}

void Renderer::reversePlayerFaceTexture(SDL_Texture* texture, SDL_FRect srcRect, SDL_FRect dstRect) const noexcept{
	//左右反转纹理
	SDL_RenderTextureRotated(renderer_.get(), texture, &srcRect, &dstRect, 0, nullptr, SDL_FLIP_HORIZONTAL);
}

