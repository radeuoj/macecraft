#undef min
#undef max
#include "Game.h"
#include "PlayerLayer.h"
#include "WorldLayer.h"
#include "TestLayer.h"
#include "jet_utils.h"

bool test() {
    Jetstream::Utils::Println("awdawd");
    return false;
}

int main()
{
    LogInfo("HELLO THERE AGAIN");
    LogWarning("This is a warnging");
    LogError("This is an error");
    // JET_ASSERT(test(), "yup, nup");
    
    Macecraft::Game game;

    // game.BindLayer<TestLayer>();
    game.BindLayer<Macecraft::PlayerLayer>();
    game.BindLayer<Macecraft::WorldLayer>();
    
    game.Run();

    return 0;
}
