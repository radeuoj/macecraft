#undef min
#undef max
#include "Game.h"
#include "PlayerLayer.h"
#include "WorldLayer.h"
#include "TestLayer.h"

int main()
{
    Macecraft::Game game;

    // game.BindLayer<TestLayer>();
    game.BindLayer<Macecraft::PlayerLayer>();
    game.BindLayer<Macecraft::WorldLayer>();
    
    game.Run();

    return 0;
}
