#pragma once

namespace Macecraft {

class Game;

class Layer
{
public:
    Layer() = delete;
    Layer(const Game& game);
    Layer(const Layer&) = delete;
    Layer(Layer&&) = delete;
    virtual ~Layer() = default;

    virtual void OnLoad() {}
    virtual void OnUnload() {}
    virtual void OnUpdate(float deltaTime) {}
    virtual void OnRender(float deltaTime) {}
    virtual void OnImGuiRender(float deltaTime) {}
protected:
    const Game& game;

};

}

