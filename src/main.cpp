#include "Game.h"
#include "TestLayer.h"

int main()
{
    Macecraft::Game game;

    // game.BindLayer<TestLayer>();
    game.BindLayer<Macecraft::WorldLayer>();
    
    game.Run();

    return 0;
}
