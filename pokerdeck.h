#ifndef POKERDECK_H
#define POKERDECK_H

#define ARRAYLEN(x)         ((ssize_t)(sizeof (x)  / sizeof *(x)))
#define HAS_FLAG(x, flag)   (((x) & (flag)) == (flag))

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

Card Card_from_id(int id);

#define CARD_TEXT(c)    (Card_from_id(c).text)

typedef struct {
    int count;
    int cards[52];
} Deck;

typedef struct {
    const char *name;
    Card cards[2];
} Hand;

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

void evaluate(Hand *hand, Board *board);

#endif
