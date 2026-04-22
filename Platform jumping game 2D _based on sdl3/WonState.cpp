#include "WonState.h"
#include "render/Renderer.h"

WonState::WonState(Renderer& renderer) noexcept : State<StateType>(StateType::WON), renderer_(renderer){

}

void WonState::render() const noexcept{
	// 切换到静态纹理渲染目标并清除内容
	renderer_.clearStaticTexture();
	// 切换到动态纹理渲染目标并清除内容
	renderer_.clearDynamicTexture(); 
}

void WonState::update(double dt) noexcept{

}
