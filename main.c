#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static const char RankSym[] = "23456789TJQKA";
static const char SuitSym[] = "shdc";

#define RANKX(c) ((c) % 13)
#define SUITX(c) ((c) / 13)

#define RANK(c) (RankSym[RANKX(c)])
#define SUIT(c) (SuitSym[SUITX(c)])

enum {
    RANK_A = 12,
};

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

typedef struct {
    const char *name;
    Card cards[2];
} Hand;

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

typedef enum {
    BOARD_EMPTY = 0,
    BOARD_FLOP = 1,
    BOARD_TURN = 2,
    BOARD_RIVER = 3,
} BoardState;

typedef struct {
    BoardState state;
    Card cards[5];
} Board;

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

void evaluate(Hand *hand, Board *board)
{
    static const int CARDLEN = 7;

    int cards[] = {
        hand->cards[0].id,
        hand->cards[1].id,
        board->cards[0].id,
        board->cards[1].id,
        board->cards[2].id,
        board->cards[3].id,
        board->cards[4].id,
    };

    for (int i = 0; i < CARDLEN - 1; ++i) {
        if (RANKX(cards[i]) == 1)
            continue;
        int lowx = i;
        for (int j = i + 1; j < CARDLEN; ++j) {
            if (RANKX(cards[j]) < RANKX(cards[lowx]))
                lowx = j;
        }
        if (lowx == i)
            continue;
        int tmp = cards[i];
        cards[i] = cards[lowx];
        cards[lowx] = tmp;
    }

    printf("Sorted Cards:\n\t");
    for (int x = 0; x < CARDLEN; ++x) {
        printf("%s ", Card_from_id(cards[x]).text);
    }
    printf("\n");

    int last_rank = (RANKX(cards[CARDLEN - 1]) == RANK_A) ? -1 : RANKX(cards[CARDLEN - 1]);
    int longest_straight = 1;
    int current_straight = 1;
    for (int i = 0; i < CARDLEN; ++i) {
        int rank = RANKX(cards[i]);
        if (rank == last_rank)
            continue;
        if (rank == last_rank + 1)
            ++current_straight;
            if (current_straight > longest_straight)
                longest_straight = current_straight;
        else
            current_straight = 1;
        last_rank = rank;
    }

    printf("Longest straight: %d\n", longest_straight);

    int suitc[4] = {0};
    for (int i = 0; i < CARDLEN; ++i) {
        int suitx = SUITX(cards[i]);
        ++suitc[suitx];
    }

    printf("Suit counts:\n");
    for (int i = 0; i < 4; ++i) {
        char flushsym = (suitc[i] >= 5) ? '*' : ' ';
        printf("\t%c%c: %d\n", flushsym, SuitSym[i], suitc[i]);
    }

    int rankc[13] = {0};
    for (int i = 0; i < CARDLEN; ++i) {
        int rankx = RANKX(cards[i]);
        ++rankc[rankx];
    }

    printf("Rank counts:\n");
    for (int i = 0; i < 13; ++i) {
        if (!rankc[i])
            continue;
        printf("\t%c: %d\n", RankSym[i], rankc[i]);
    }
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
