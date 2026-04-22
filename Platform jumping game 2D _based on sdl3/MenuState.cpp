#include "MenuState.h"
#include "render/Renderer.h"

MenuState::MenuState(Renderer& r) noexcept : State<StateType>(StateType::MENU), renderer_(r){

}

void MenuState::update(double dt) noexcept{

}

void MenuState::render() const noexcept{
	// 切换到静态纹理渲染目标并清除内容，准备渲染地图等静态元素
	renderer_.clearStaticTexture(); 

	// 切换到动态纹理渲染目标并清除内容，准备渲染胜利界面等动态元素
	renderer_.clearDynamicTexture(); 
}
