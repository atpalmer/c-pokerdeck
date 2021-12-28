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
    int id;
    char text[3];  /* '\0'-terminated string */
} Card;

Card Card_from_id(int c)
{
    Card result = {
        .id = c,
        .text = {RANK(c), SUIT(c), '\0'},
    };
    return result;
}

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

Card Deck_next(Deck *this)
{
    int id = this->cards[this->count - 1];
    --this->count;
    return Card_from_id(id);
}

void deal_player(Deck *deck, const char *name)
{
    Card hand[] = {Deck_next(deck), Deck_next(deck)};
    printf("%s:\n", name);
    printf("\t%s %s\n", hand[0].text, hand[1].text);
}

void deal_board(Deck *deck)
{
    Card flop[] = {
        Deck_next(deck),
        Deck_next(deck),
        Deck_next(deck),
    };

    printf("Flop:\n");
    printf("\t%s %s %s\n",
        flop[0].text, flop[1].text, flop[2].text);

    Card turn = Deck_next(deck);

    printf("Turn:\n");
    printf("\t%s %s %s %s\n",
        flop[0].text, flop[1].text, flop[2].text, turn.text);

    Card river = Deck_next(deck);

    printf("River:\n");
    printf("\t%s %s %s %s %s\n",
        flop[0].text, flop[1].text, flop[2].text, turn.text, river.text);
}

int main(void)
{
    Deck *deck = Deck_new();
    Deck_shuffle(deck);
    deal_player(deck, "Hero");
    deal_board(deck);
    Deck_cleanup(deck);
}
