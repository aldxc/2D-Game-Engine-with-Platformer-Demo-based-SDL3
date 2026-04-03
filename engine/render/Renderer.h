#pragma once
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

//单例模式
class Renderer {
public:
	static Renderer& getInstance() {
		static Renderer instance;
		return instance;
	}

	bool init(int w, int h) noexcept;

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(Renderer&&) = delete;

	void beginRender() const noexcept;
	void restoreDefaultAndPresent() const noexcept;

	void renderRect(const SDL_FRect& rect, const SDL_Color& color) const noexcept;
	void renderText(const std::string text, const SDL_FRect& rect, const SDL_Color color = SDL_Color({ 100, 100, 100, 255 }), const int t_size = 30) const noexcept;

private:
	Renderer();
	~Renderer();
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
private:
	std::unique_ptr<SDL_Window, SDL_Deleter> window_;
	std::unique_ptr<SDL_Renderer, SDL_RendererDeleter> renderer_;
	std::unique_ptr<TTF_TextEngine, SDL_TextEngineDeleter> textEngine_;
	std::unique_ptr<TTF_Font, SDL_FontDeleter> font_;
	mutable std::unique_ptr<TTF_Text, TTF_TEXTDeleter> text_; 
};