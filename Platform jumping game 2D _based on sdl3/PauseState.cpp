#include "PauseState.h"
#include "render/Renderer.h"

PauseState::PauseState(Renderer& renderer) noexcept : State<StateType>(StateType::PAUSE), renderer_(renderer) {

}

void PauseState::update(double dt) noexcept{

}

void PauseState::render() const noexcept{

}
