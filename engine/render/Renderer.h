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

	void beginRender() const noexcept;
	void restoreDefaultAndPresent() const noexcept;

	void renderRect(const SDL_FRect& rect, const SDL_Color& color) const noexcept;
	void renderFillRect(const SDL_FRect& rect, const SDL_Color& color) const noexcept;
	void renderText(const std::string text, const SDL_FRect& rect, const SDL_Color color = SDL_Color({ 100, 100, 100, 255 }), const int t_size = 30) const noexcept;
	void renderTexture(SDL_Texture* texture, const Rect& srcrect, const Rect& dstrect) const noexcept;
	
	void resetRenderTarget() noexcept; // 重置渲染目标为默认，适用于结束UI、地图等元素的渲染后恢复到默认渲染目标准备渲染玩家、敌人等动态元素
	void clearUITexture(const SDL_Color color = SDL_Color({ 0, 0, 0, 0 })) noexcept; // 切换到UI纹理渲染目标并清除纹理内容，适用于UI元素的渲染准备，保持UI渲染质量同时避免与地图、玩家等动态元素的纹理混用带来的性能问题
	void clearStaticTexture(const SDL_Color color = SDL_Color({ 0, 0, 0, 0 })) noexcept;//切换到静态纹理渲染目标并清除纹理内容，适用于UI、地图等元素的渲染准备
	void clearDynamicTexture(const SDL_Color color = SDL_Color({ 0, 0, 0, 0 })) noexcept;//切换到动态纹理渲染目标并清除纹理内容，适用于玩家、敌人等频繁更新的元素的渲染准备

	void renderStaticTexture() const noexcept; // 将静态纹理渲染到默认渲染目标，适用于UI、地图等元素的最终呈现
	void renderDynamicTexture() const noexcept; // 将动态纹理渲染到默认渲染目标，适用于玩家、敌人等频繁更新的元素的最终呈现
	void renderUITexture() const noexcept; // 将UI纹理渲染到默认渲染目标，适用于UI元素的最终呈现

	SDL_Renderer* getSDLRenderer() const noexcept { return renderer_.get(); } // 获取底层SDL_Renderer指针，适用于需要直接使用SDL_Renderer进行高级渲染操作的场景，例如使用SDL_RenderCopyEx进行旋转缩放等变换操作

	void reversePlayerFaceTexture(SDL_Texture* texture, const Rect& srcRect, const Rect& dstRect) const noexcept;
	SDL_Color setColorAlpha(uint8_t r, uint8_t g, uint8_t b, uint8_t a) const noexcept { return SDL_Color{ r, g, b, a }; } // 外界使用此函数设置颜色及alpha值，统一接口防止直接使用SDL_Color导致的混乱

private:
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
			if (text) TTF_DestroyText(text); // SDL_ttf 文本对象销毁
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
	//mutable std::unique_ptr<TTF_Text, TTF_TEXTDeleter> text_;  // 使用智能缓存文本对象，避免频繁创建销毁带来的性能问题，同时确保线程安全，后续可使用文字图集等优化方案
	std::unique_ptr<SDL_Texture, SDL_TextureDeleter> UITexture_; // 专门用于UI元素的渲染，避免与地图、玩家等动态元素的纹理混用带来的性能问题，同时保持渲染质量
	std::unique_ptr<SDL_Texture, SDL_TextureDeleter> staticTexture_; // 用于静态纹理渲染，避免每帧创建销毁纹理带来的性能问题，包括UI、map等 // 后续可依据更新频率分离
	std::unique_ptr<SDL_Texture, SDL_TextureDeleter> dynamicTexture_; // 用于动态纹理渲染，适用于玩家、敌人等频繁更新的元素，优化性能同时保持渲染质量
	mutable std::unordered_map<std::string, std::unique_ptr<TTF_Text, TTF_TEXTDeleter>> textCache_; // 文本缓存，键为文本内容和属性的组合，值为对应的TTF_Text对象，避免重复创建相同文本对象带来的性能问题
};