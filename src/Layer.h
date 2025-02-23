#pragma once

namespace Macecraft {

class Game;

class Layer
{
public:
    Layer() = default;
    // Layer(const Game& game);
    Layer(const Layer&) = delete;
    Layer(Layer&&) = delete;
    virtual ~Layer() = default;

    virtual void OnLoad() {}
    virtual void OnUnload() {}
    virtual void OnUpdate(float deltaTime) {}
    virtual void OnRender(float deltaTime) {}
    virtual void OnUIRender(float deltaTime) {}
    virtual void OnImGuiRender(float deltaTime) {}

    void __Internal_SetGame(const Game* __game);
protected:
    const Game* game = nullptr;

};

}

