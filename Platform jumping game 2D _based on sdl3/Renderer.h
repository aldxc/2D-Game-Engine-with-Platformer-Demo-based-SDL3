#pragma once
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

//µ¥ÀýÄ£Ê½
class Renderer {
public:
	static Renderer& getInstance() {
		static Renderer instance;
		return instance;
	}

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(Renderer&&) = delete;

	void beginRender() const noexcept;
	void restoreDefaultAndPresent() const noexcept;

	void renderRect(const SDL_FRect& rect, const SDL_Color& color) const noexcept;

private:
	Renderer();
	~Renderer();
	bool init() noexcept;
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
private:
	std::unique_ptr<SDL_Window, SDL_Deleter> window_;
	std::unique_ptr<SDL_Renderer, SDL_RendererDeleter> renderer_;
};