#include "LoseState.h"
#include "render/Renderer.h"

LoseState::LoseState() noexcept : State<StateType>(StateType::LOSE) {

}

void LoseState::render() const noexcept {
	Renderer::getInstance().clearStaticTexture(); // 切换到静态纹理渲染目标并清除内容，准备渲染地图等静态元素

	Renderer::getInstance().clearDynamicTexture(); // 切换到动态纹理渲染目标并清除内容，准备渲染胜利界面等动态元素
}

void LoseState::update(float dt) noexcept {

}
