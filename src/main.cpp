#include "Game.h"

int main()
{
    Macecraft::Game* game = new Macecraft::Game();
    game->run();
    delete game;

    return 0;
}