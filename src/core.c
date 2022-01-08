#include <stdlib.h>
#include "pokerdeck.h"


/*** Deck ***/

Deck *Deck_new_ordered(void)
{
    Deck *new = malloc(sizeof *new);
    if (!new)
        return NULL;
    for (int i = 0; i < 52; ++i) {
        new->cards[i] = i;
    }
    new->count = 52;
    return new;
}

void Deck_destroy(Deck *this)
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

Deck *Deck_new_shuffled(void)
{
    Deck *new = Deck_new_ordered();
    if (!new)
        return NULL;
    Deck_shuffle(new);
    return new;
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


/*** Game ***/

Game *Game_new(void)
{
    Game *new = malloc(sizeof *new);
    if (!new)
        return NULL;
    new->deck = Deck_new_shuffled();
    new->hero = Player_deal("Hero", new->deck);
    new->villain = Player_deal("Villain", new->deck);
    new->board = Board_new();
    return new;
}

void Game_destroy(Game *this)
{
    Deck_destroy(this->deck);
    free(this);
}

void Game_fill_board(Game *this)
{
    Board_fill(&this->board, this->deck);
}

void Game_evaluate_hands(Game *this)
{
    Player_evaluate(&this->hero, &this->board);
    Player_evaluate(&this->villain, &this->board);
}

Game *Game_new_runout(void)
{
    Game *new = Game_new();
    if (!new)
        return NULL;
    Game_fill_board(new);
    Game_evaluate_hands(new);
    return new;
}

