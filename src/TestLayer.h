#pragma once
#include "Layer.h"

class TestLayer: public Macecraft::Layer
{
public:
    using Layer::Layer;

    void OnLoad() override;
    void OnUnload() override;
    void OnUpdate(float deltaTime) override;
    void OnRender(float deltaTime) override;
    void OnImGuiRender(float deltaTime) override;
    
};
