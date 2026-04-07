#include "WonState.h"
#include "render/Renderer.h"

WonState::WonState() noexcept : State<StateType>(StateType::WON){

}

void WonState::render() const noexcept{
	Renderer::getInstance().clearStaticTexture(); // 切换到静态纹理渲染目标并清除内容，准备渲染地图等静态元素

	Renderer::getInstance().clearDynamicTexture(); // 切换到动态纹理渲染目标并清除内容，准备渲染胜利界面等动态元素
}

void WonState::update(float dt) noexcept{

}
