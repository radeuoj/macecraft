#include "Game.h"
#include "TestLayer.h"

int main()
{
    Macecraft::Game game;

    TestLayer* testLayer = game.BindLayer<TestLayer>();
    testLayer->OnLoad();
    
    game.Run();

    return 0;
}
