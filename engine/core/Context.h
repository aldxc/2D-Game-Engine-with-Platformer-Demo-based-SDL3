#pragma once

class Renderer;
class Camera;

struct RenderContext {
	Renderer& renderer; // 渲染器引用
	Camera& camera; // 摄像机引用
};