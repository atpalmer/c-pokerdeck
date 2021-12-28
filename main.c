#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static const char RankSym[] = "23456789TJQKA";
static const char SuitSym[] = "shdc";

#define RANKX(c) ((c) % 13)
#define SUITX(c) ((c) / 13)

#define RANK(c) (RankSym[RANKX(c)])
#define SUIT(c) (SuitSym[SUITX(c)])

typedef struct {
    int count;
    int cards[52];
} Deck;

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

int Deck_next(Deck *this)
{
    int result = this->cards[this->count - 1];
    --this->count;
    return result;
}

int main(void)
{
    Deck *deck = Deck_new();
    Deck_shuffle(deck);
    while (deck->count) {
        int card = Deck_next(deck);
        printf("Card: %c%c (%d)\n", RANK(card), SUIT(card), card);
    }
    Deck_cleanup(deck);
}
