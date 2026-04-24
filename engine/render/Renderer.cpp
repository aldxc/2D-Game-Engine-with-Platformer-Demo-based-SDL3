#include "core/Rect.h"
#include "Renderer.h"

Renderer::~Renderer() noexcept {
	m_textCache.clear();
	m_textEngine.reset();
	m_font.reset();
	m_renderer.reset();
	m_window.reset();

	TTF_Quit();
	SDL_Quit();
}

bool Renderer::init(int w, int h, int logicW, int logicH, int fontSize) noexcept{
	SDL_Window* window = SDL_CreateWindow("Platform Jumping Game 2D", w, h, 0);
	if (!window) {
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	m_window.reset(window);

	SDL_Renderer* renderer = SDL_CreateRenderer(m_window.get(), nullptr);
	if (!renderer) {
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}
	m_renderer.reset(renderer);
	SDL_SetRenderTarget(m_renderer.get(), nullptr);
	SDL_SetRenderLogicalPresentation(m_renderer.get(), logicW, logicH, SDL_LOGICAL_PRESENTATION_LETTERBOX); 

	TTF_TextEngine* textEngine = TTF_CreateRendererTextEngine(m_renderer.get());
	if (!textEngine) {
		SDL_Log("Failed to create TextEngine: %s", SDL_GetError());
		return false;
	}
	m_textEngine.reset(textEngine);

	std::string path = "resource/font.ttf";
	TTF_Font* font = TTF_OpenFont(path.c_str(), fontSize);
	if (!font) {
		SDL_Log("Failed to open font: %s", SDL_GetError());
		return false;
	}
	m_font.reset(font);

	SDL_Texture* UITexture = SDL_CreateTexture(m_renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, logicW, logicH);
	if(!UITexture) {
		SDL_Log("Failed to create UI texture: %s", SDL_GetError());
		return false;
	}
	m_UITexture.reset(UITexture);

	SDL_Texture* staticTexture = SDL_CreateTexture(m_renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, logicW, logicH);
	if(!staticTexture) {
		SDL_Log("Failed to create static texture: %s", SDL_GetError());
		return false;
	}
	m_staticTexture.reset(staticTexture);

	SDL_Texture* dynamicTexture = SDL_CreateTexture(m_renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, logicW, logicH);
	if(!dynamicTexture) {
		SDL_Log("Failed to create dynamic texture: %s", SDL_GetError());
		return false;
	}
	m_dynamicTexture.reset(dynamicTexture);

	return true;
}

void Renderer::beginRender() const noexcept{
	SDL_SetRenderDrawColor(m_renderer.get(), 0, 0, 0, 255);
	SDL_RenderClear(m_renderer.get());
}

void Renderer::restoreDefaultAndPresent() const noexcept {
	SDL_SetRenderTarget(m_renderer.get(), nullptr);

	//重置字体
	TTF_SetFontSize(m_font.get(), 30);
	SDL_RenderPresent(m_renderer.get());
}

void Renderer::renderRect(const SDL_FRect& rect, const SDL_Color& color) const noexcept{
	SDL_SetRenderDrawColor(m_renderer.get(), color.r, color.g, color.b, color.a);
	SDL_RenderRect(m_renderer.get(), &rect);
}

void Renderer::renderFillRect(const SDL_FRect& rect, const SDL_Color& color) const noexcept{
	SDL_SetRenderDrawColor(m_renderer.get(), color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(m_renderer.get(), &rect);
}

void Renderer::renderText(const std::string text, const SDL_FRect& rect, const SDL_Color color, const int t_size) const noexcept{
	TTF_SetFontSize(m_font.get(), t_size);
	std::unique_ptr<TTF_Text, TTF_TEXTDeleter> m_text;

	auto it = m_textCache.find(text);
	if(it == m_textCache.end()) {
		// 没有找到缓存，创建新的文本对象
		TTF_Text* t = TTF_CreateText(m_textEngine.get(), m_font.get(), text.c_str(), text.size());
		if (!t) {
			SDL_Log("Fail to create text : %s", SDL_GetError());
			return;
		}
		m_text.reset(t);
	} else {
		m_text = std::move(it->second);
	}
	
	TTF_SetTextColor(m_text.get(), color.r, color.g, color.b, color.a);
	int w = 0, h = 0;
	TTF_GetTextSize(m_text.get(), &w, &h);
	//居中绘制
	float x = rect.x + (rect.w - w) / 2.0f;
	float y = rect.y + (rect.h - h) / 2.0f;
	TTF_DrawRendererText(m_text.get(), x, y);

	m_textCache[text] = std::move(m_text);
}

void Renderer::renderTexture(SDL_Texture* texture, const Rect& srcrect, const Rect& dstrect) const noexcept {
	SDL_FRect src = srcrect;
	SDL_FRect dst = dstrect;
	SDL_RenderTexture(m_renderer.get(), texture, &src, &dst);
}

void Renderer::resetRenderTarget() noexcept{
	SDL_SetRenderTarget(m_renderer.get(), nullptr);
	SDL_SetRenderLogicalPresentation(m_renderer.get(), 1024, 576, SDL_LOGICAL_PRESENTATION_LETTERBOX);
}

void Renderer::clearUITexture(const SDL_Color color) noexcept{
	SDL_SetRenderTarget(m_renderer.get(), m_UITexture.get());

	SDL_SetRenderDrawColor(m_renderer.get(), color.r, color.g, color.b, color.a);
	SDL_RenderClear(m_renderer.get());
}

void Renderer::clearStaticTexture(const SDL_Color color) noexcept{
	SDL_SetRenderTarget(m_renderer.get(), m_staticTexture.get());

	SDL_SetRenderDrawColor(m_renderer.get(), color.r, color.g, color.b, color.a);
	SDL_RenderClear(m_renderer.get());
}

void Renderer::clearDynamicTexture(const SDL_Color color) noexcept{
	SDL_SetRenderTarget(m_renderer.get(), m_dynamicTexture.get());

	SDL_SetRenderDrawColor(m_renderer.get(), color.r, color.g, color.b, color.a);
	SDL_RenderClear(m_renderer.get());
}

void Renderer::renderStaticTexture() const noexcept{
	// 开启混合模式以支持动态纹理的透明度
	SDL_SetRenderDrawBlendMode(m_renderer.get(), SDL_BLENDMODE_BLEND); 
	SDL_RenderTexture(m_renderer.get(), m_staticTexture.get(), nullptr, nullptr);

}

void Renderer::renderDynamicTexture() const noexcept{
	// 开启混合模式以支持动态纹理的透明度
	SDL_SetRenderDrawBlendMode(m_renderer.get(), SDL_BLENDMODE_BLEND); 
	SDL_RenderTexture(m_renderer.get(), m_dynamicTexture.get(), nullptr, nullptr);
}

void Renderer::renderUITexture() const noexcept{
	// 开启混合模式以支持动态纹理的透明度
	SDL_SetRenderDrawBlendMode(m_renderer.get(), SDL_BLENDMODE_BLEND); 
	SDL_RenderTexture(m_renderer.get(), m_UITexture.get(), nullptr, nullptr);
}

void Renderer::reversePlayerFaceTexture(SDL_Texture* texture, const Rect& srcRect, const Rect& dstRect) const noexcept{
	//左右反转纹理
	SDL_FRect src = srcRect;
	SDL_FRect dst = dstRect;
	SDL_RenderTextureRotated(m_renderer.get(), texture, &src, &dst, 0, nullptr, SDL_FLIP_HORIZONTAL);
}

