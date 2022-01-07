#ifndef POKERDECK_H
#define POKERDECK_H

#include <sys/types.h>

#define ARRAYLEN(x)         ((ssize_t)(sizeof (x)  / sizeof *(x)))
#define HAS_FLAG(x, flag)   (((x) & (flag)) == (flag))

static const char RankSym[] = "23456789TJQKA";
static const char SuitSym[] = "shdc";

#define RANKX(c) ((c) % 13)
#define SUITX(c) ((c) / 13)

#define CARD_RANKSYM(c) (RankSym[RANKX(c)])
#define CARD_SUITSYM(c) (SuitSym[SUITX(c)])

enum {
    RANK_A = 12,
};

typedef struct {
    char value[3];  /* '\0'-terminated string */
} CardString;

static inline CardString CardString_from_id(int c)
{
    CardString result = {{CARD_RANKSYM(c), CARD_SUITSYM(c), '\0'}};
    return result;
}

#define CARD_TEXT(c)    (CardString_from_id(c).value)

typedef struct {
    int count;
    int cards[52];
} Deck;

typedef enum {
    BOARD_EMPTY = 0,
    BOARD_FLOP = 1,
    BOARD_TURN = 2,
    BOARD_RIVER = 3,
} BoardState;

typedef struct {
    BoardState state;
    int cards[5];
} Board;

/*
 * Hand evaluation format encodes leading hand qualification,
 * followed by ordered card ranks (4 bits x 5 cards).
 */
typedef uint32_t HandEval;

enum {
    EVAL_NONE       = 0x00000000,
    EVAL_PAIR       = 0x00100000,
    EVAL_TWOPAIR    = 0x00200000,
    EVAL_TRIPS      = 0x00300000,
    EVAL_STRAIGHT   = 0x00400000,
    EVAL_FLUSH      = 0x00500000,
    EVAL_BOAT       = 0x00600000,
    EVAL_QUADS      = 0x00700000,
    EVAL_STFLUSH    = 0x00800000,
    EVAL_ROYAL      = 0x009cba98,
};

#define EVALX(e)    ((e) >> (4 * 5))

static const char *_EVALX_TEXT[] = {
    [EVALX(EVAL_NONE)]     = "High card",
    [EVALX(EVAL_PAIR)]     = "Pair",
    [EVALX(EVAL_TWOPAIR)]  = "Two Pair",
    [EVALX(EVAL_TRIPS)]    = "Three of a Kind",
    [EVALX(EVAL_STRAIGHT)] = "Straight",
    [EVALX(EVAL_FLUSH)]    = "Flush",
    [EVALX(EVAL_BOAT)]     = "Full House",
    [EVALX(EVAL_QUADS)]    = "Four of a Kind",
    [EVALX(EVAL_STFLUSH)]  = "Straight Flush",
    [EVALX(EVAL_ROYAL)]    = "Royal Flush",
};

#define EVAL_TEXT(e)            (_EVALX_TEXT[EVALX(e)])
#define EVAL_GETRANK(e, pos)    (((e) >> ((4 - (pos)) * 4)) & 0x000000f)

HandEval evaluate(int *handcards, int *boardcards);

typedef struct {
    const char *name;
    int cards[2];
    HandEval eval;
} Player;

#endif
