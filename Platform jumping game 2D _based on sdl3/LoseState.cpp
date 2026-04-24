#include "LoseState.h"
#include "render/Renderer.h"

LoseState::LoseState(Renderer& renderer) noexcept : State<StateType>(StateType::LOSE), m_renderer(renderer) {

}

void LoseState::render() const noexcept {
	// 切换到静态纹理渲染目标并清除内容
	m_renderer.clearStaticTexture(); 

	// 切换到动态纹理渲染目标并清除内容
	m_renderer.clearDynamicTexture(); 
}

void LoseState::update(double dt) noexcept {

}
