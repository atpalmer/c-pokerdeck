#include <stdio.h>
#include <stdlib.h>
#include "pokerdeck.h"


/*** Deck ***/

Deck *Deck_new(void)
{
    Deck *new = malloc(sizeof *new);
    for (int i = 0; i < 52; ++i) {
        new->cards[i] = i;
    }
    new->count = 52;
    return new;
}

void Deck_cleanup(Deck *this)
{
    free(this);
}

void Deck_shuffle(Deck *this)
{
    for (int i = 0; i < this->count; ++i) {
        int newpos = rand() % this->count;
        int tmp = this->cards[newpos];
        this->cards[newpos] = this->cards[i];
        this->cards[i] = tmp;
    }
}

int Deck_next(Deck *this)
{
    int id = this->cards[this->count - 1];
    --this->count;
    return id;
}


/*** Player ***/

Player Player_deal(const char *name, Deck *deck)
{
    Player result = {
        .name = name,
        .cards = {
            Deck_next(deck),
            Deck_next(deck),
        },
        .eval = EVAL_NONE,
    };
    return result;
}

void Player_evaluate(Player *p, Board *b)
{
    p->eval = evaluate(p->cards, b->cards);
}

void Player_show_hand(Player *p)
{
    printf("%s:\n", p->name);
    printf("\t%s %s\n", CARD_TEXT(p->cards[0]), CARD_TEXT(p->cards[1]));
}

void Player_show_eval(Player *p)
{
    printf("%10s: %s [", p->name, EVAL_TEXT(p->eval));
    for (int i = 0; i < 5; ++i) {
        int rankx = EVAL_GETRANK(p->eval, i);
        printf("%c", RankSym[rankx]);
    }
    printf("]\n");
}


/*** Board ***/

Board Board_new(void)
{
    Board result = {
        .state = BOARD_EMPTY,
        .cards = {0},
    };
    return result;
}

/**
 * Returns num cards dealt (0 or 3)
 */
int Board_flop(Board *this, Deck *deck)
{
    if (this->state != BOARD_EMPTY)
        return 0;
    this->cards[0] = Deck_next(deck);
    this->cards[1] = Deck_next(deck);
    this->cards[2] = Deck_next(deck);
    this->state = BOARD_FLOP;
    return 3;
}

int Board_turn(Board *this, Deck *deck)
{
    if (this->state != BOARD_FLOP)
        return 0;
    this->cards[3] = Deck_next(deck);
    this->state = BOARD_TURN;
    return 1;
}

int Board_river(Board *this, Deck *deck)
{
    if (this->state != BOARD_TURN)
        return 0;
    this->cards[4] = Deck_next(deck);
    this->state = BOARD_RIVER;
    return 1;
}

void Board_fill(Board *this, Deck *deck)
{
    for (int i = 0; i < 5; ++i) {
        this->cards[i] = Deck_next(deck);
    }
    this->state = BOARD_RIVER;
}

void Board_deal_streets(Board *board, Deck *deck)
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


/*** Game ***/

Game *Game_new(void)
{
    Game *new = malloc(sizeof *new);

    new->deck = Deck_new();
    Deck_shuffle(new->deck);

    new->hero = Player_deal("Hero", new->deck);
    new->villain = Player_deal("Villain", new->deck);
    new->board = Board_new();

    return new;
}

void Game_destroy(Game *this)
{
    Deck_cleanup(this->deck);
    free(this);
}

void Game_deal_board(Game *this)
{
    Board_deal_streets(&this->board, this->deck);
}

void Game_fill_board(Game *this)
{
    Board_fill(&this->board, this->deck);
}

void Game_show_winner(Game *this)
{
    Player *winner = WINNING_PLAYER(&this->hero, &this->villain);
    printf("%10s: %s\n", "Winner", winner ? winner->name : "Chop");
}

