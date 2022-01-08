#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pokerdeck.h"

static void Player_show_hand(Player *p)
{
    printf("%s:\n", p->name);
    printf("\t%s %s\n", CARD_TEXT(p->cards[0]), CARD_TEXT(p->cards[1]));
}

static void Player_show_eval(Player *p)
{
    printf("%10s: %s [", p->name, EVAL_TEXT(p->eval));
    for (int i = 0; i < 5; ++i) {
        int rankx = EVAL_GETRANK(p->eval, i);
        printf("%c", RankSym[rankx]);
    }
    printf("]\n");
}

static void Board_deal_streets(Board *board, Deck *deck)
{
    if (!Board_flop(board, deck))
        return;

    printf("Flop:\n");
    printf("\t%s %s %s\n",
        CARD_TEXT(board->cards[0]),
        CARD_TEXT(board->cards[1]),
        CARD_TEXT(board->cards[2]));

    if (!Board_turn(board, deck))
        return;

    printf("Turn:\n");
    printf("\t%s %s %s %s\n",
        CARD_TEXT(board->cards[0]),
        CARD_TEXT(board->cards[1]),
        CARD_TEXT(board->cards[2]),
        CARD_TEXT(board->cards[3]));

    if (!Board_river(board, deck))
        return;

    printf("River:\n");
    printf("\t%s %s %s %s %s\n",
        CARD_TEXT(board->cards[0]),
        CARD_TEXT(board->cards[1]),
        CARD_TEXT(board->cards[2]),
        CARD_TEXT(board->cards[3]),
        CARD_TEXT(board->cards[4]));
}

static void Game_show_result(Game *game)
{
    printf("Result:\n");
    Player_show_eval(&game->hero);
    Player_show_eval(&game->villain);

    Player *winner = GAME_WINNER(game);
    printf("%10s: %s\n", "Winner", winner ? winner->name : "Chop");
}

int main(void)
{
    srand(time(NULL));

    Game *game = Game_new();

    Player_show_hand(&game->hero);
    Player_show_hand(&game->villain);
    Board_deal_streets(&game->board, game->deck);
    Game_evaluate_hands(game);
    Game_show_result(game);

    Game_destroy(game);
}
