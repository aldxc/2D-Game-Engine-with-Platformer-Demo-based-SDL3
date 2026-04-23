#pragma once
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <unordered_map>
#include "core/Rect.h"

class Renderer {
public:
	explicit Renderer() noexcept = default;
	~Renderer() noexcept ;
	bool init(int w, int h, int logicW, int logicH, int fontSize) noexcept;

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(Renderer&&) = delete;

	// 渲染流程控制
	void beginRender() const noexcept;
	void restoreDefaultAndPresent() const noexcept;

	// 基本渲染接口
	void renderRect(const SDL_FRect& rect, const SDL_Color& color) const noexcept;
	void renderFillRect(const SDL_FRect& rect, const SDL_Color& color) const noexcept;
	void renderText(const std::string text, const SDL_FRect& rect, const SDL_Color color = SDL_Color({ 100, 100, 100, 255 }), const int t_size = 30) const noexcept;
	void renderTexture(SDL_Texture* texture, const Rect& srcrect, const Rect& dstrect) const noexcept;

	// 重置渲染目标为默认
	void resetRenderTarget() noexcept; 
	// 切换到UI纹理渲染目标并清除纹理内容
	void clearUITexture(const SDL_Color color = SDL_Color({ 0, 0, 0, 0 })) noexcept; 
	//切换到静态纹理渲染目标并清除纹理内容
	void clearStaticTexture(const SDL_Color color = SDL_Color({ 0, 0, 0, 0 })) noexcept;
	//切换到动态纹理渲染目标并清除纹理内容
	void clearDynamicTexture(const SDL_Color color = SDL_Color({ 0, 0, 0, 0 })) noexcept;

	// 将静态纹理渲染到默认渲染目标
	void renderStaticTexture() const noexcept; 
	// 将动态纹理渲染到默认渲染目标
	void renderDynamicTexture() const noexcept; 
	// 将UI纹理渲染到默认渲染目标
	void renderUITexture() const noexcept; 

	// 获取底层renderer指针
	SDL_Renderer* getSDLRenderer() const noexcept { return renderer_.get(); } 

	void reversePlayerFaceTexture(SDL_Texture* texture, const Rect& srcRect, const Rect& dstRect) const noexcept;
	// 外界使用此函数设置颜色及alpha值，统一接口防止直接使用SDL_Color导致的混乱
	SDL_Color setColorAlpha(uint8_t r, uint8_t g, uint8_t b, uint8_t a) const noexcept { return SDL_Color{ r, g, b, a }; } 

private:
	// 自定义删除器
	struct SDL_Deleter {
		void operator()(SDL_Window* window) const {
			SDL_DestroyWindow(window);
		}
	};
	struct SDL_RendererDeleter {
		void operator()(SDL_Renderer* renderer) const {
			SDL_DestroyRenderer(renderer);
		}
	};
	struct SDL_TextEngineDeleter {
		void operator()(TTF_TextEngine* engine) const {
			TTF_DestroyRendererTextEngine(engine);
		}
	};
	struct SDL_FontDeleter {
		void operator()(TTF_Font* font) const {
			TTF_CloseFont(font);
		}
	};
	struct TTF_TEXTDeleter {
		void operator()(TTF_Text* text) const noexcept {
			if (text) TTF_DestroyText(text); 
		}
	};
	struct SDL_TextureDeleter {
		void operator()(SDL_Texture* texture) const {
			SDL_DestroyTexture(texture);
		}
	};
private:
	std::unique_ptr<SDL_Window, SDL_Deleter> window_;
	std::unique_ptr<SDL_Renderer, SDL_RendererDeleter> renderer_;
	std::unique_ptr<TTF_TextEngine, SDL_TextEngineDeleter> textEngine_;
	std::unique_ptr<TTF_Font, SDL_FontDeleter> font_;
	// 三个纹理集
	std::unique_ptr<SDL_Texture, SDL_TextureDeleter> UITexture_; 
	std::unique_ptr<SDL_Texture, SDL_TextureDeleter> staticTexture_; 
	std::unique_ptr<SDL_Texture, SDL_TextureDeleter> dynamicTexture_; 
	// 文本缓存，键为文本内容，值为对应的TTF_Text对象，避免重复创建相同文本对象带来的性能问题
	mutable std::unordered_map<std::string, std::unique_ptr<TTF_Text, TTF_TEXTDeleter>> textCache_; 
};