#include "TestLayer.h"
#include <iostream>

void TestLayer::OnLoad()
{
    std::cout << "TestLayer OnLoad\n";
}

void TestLayer::OnUnload()
{
    std::cout << "TestLayer OnUnLoad\n";
}

void TestLayer::OnUpdate(float deltaTime)
{
    std::cout << "TestLayer OnUpdate " << deltaTime << "\n";
}

void TestLayer::OnRender(float deltaTime)
{
    std::cout << "TestLayer OnRender " << deltaTime << "\n";
}

void TestLayer::OnImGuiRender(float deltaTime)
{
    std::cout << "TestLayer OnImGuiRender " << deltaTime << "\n";
}

