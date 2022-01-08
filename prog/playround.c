#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pokerdeck.h"


int main(void)
{
    srand(time(NULL));

    Game *game = Game_new();

    Player_show_hand(&game->hero);
    Player_show_hand(&game->villain);

    Game_deal_board(game);
    Player_evaluate(&game->hero, &game->board);
    Player_evaluate(&game->villain, &game->board);

    printf("Result:\n");
    Player_show_eval(&game->hero);
    Player_show_eval(&game->villain);

    Game_show_winner(game);

    Game_destroy(game);
}
