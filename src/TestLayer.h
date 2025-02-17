#pragma once
#include "Layer.h"
#include <iostream>

class TestLayer: public Macecraft::Layer
{
public:
    using Layer::Layer;
    
    void OnLoad() override;
    
};
