#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pokerdeck.h"


/* Card */

Card Card_from_id(int c)
{
    Card result = {
        .id = c,
        .text = {RANK(c), SUIT(c), '\0'},
    };
    return result;
}


/* Deck */

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
    srand(time(NULL));
    for (int i = 0; i < this->count; ++i) {
        int newpos = rand() % this->count;
        int tmp = this->cards[newpos];
        this->cards[newpos] = this->cards[i];
        this->cards[i] = tmp;
    }
}

Card Deck_next(Deck *this)
{
    int id = this->cards[this->count - 1];
    --this->count;
    return Card_from_id(id);
}


/* Hand */

Hand Hand_deal(const char *name, Deck *deck)
{
    Hand result = {
        .name = name,
        .cards = {
            Deck_next(deck),
            Deck_next(deck),
        },
    };
    return result;
}

void Hand_show(Hand *hand)
{
    printf("%s:\n", hand->name);
    printf("\t%s %s\n", hand->cards[0].text, hand->cards[1].text);
}


/* Board */

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

void deal_board(Board *board, Deck *deck)
{
    if (!Board_flop(board, deck))
        return;

    printf("Flop:\n");
    printf("\t%s %s %s\n",
        board->cards[0].text,
        board->cards[1].text,
        board->cards[2].text);

    if (!Board_turn(board, deck))
        return;

    printf("Turn:\n");
    printf("\t%s %s %s %s\n",
        board->cards[0].text,
        board->cards[1].text,
        board->cards[2].text,
        board->cards[3].text);

    if (!Board_river(board, deck))
        return;

    printf("River:\n");
    printf("\t%s %s %s %s %s\n",
        board->cards[0].text,
        board->cards[1].text,
        board->cards[2].text,
        board->cards[3].text,
        board->cards[4].text);
}


int main(void)
{
    Deck *deck = Deck_new();
    Deck_shuffle(deck);
    Hand hero = Hand_deal("Hero", deck);
    Board board = Board_new();
    Hand_show(&hero);
    deal_board(&board, deck);
    evaluate(&hero, &board);
    Deck_cleanup(deck);
}
