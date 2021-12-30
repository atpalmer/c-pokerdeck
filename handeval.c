#include <stdio.h>
#include <stdlib.h>
#include "pokerdeck.h"

/*
 * Hand evaluation format encodes leading hand qualification,
 * followed by ordered card ranks (4 bits x 5 cards).
 */
typedef uint32_t HandEval;

enum {
    EVAL_NONE       = 0,
    EVAL_PAIR       = 0x00100000,
    EVAL_TWOPAIR    = 0x00200000,
    EVAL_TRIPS      = 0x00300000,
    EVAL_STRAIGHT   = 0x00400000,
    EVAL_FLUSH      = 0x00500000,
    EVAL_BOAT       = 0x00600000,
    EVAL_QUADS      = 0x00700000,
    EVAL_STFLUSH    = 0x00800000,
    EVAL_ROYAL      = 0x00900000,
};

#define EVALX(e)    ((e) >> (4 * 5))

static const char *_EVAL_TEXT[] = {
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

#define EVAL_TEXT(e)    (_EVAL_TEXT[EVALX(e)])

typedef struct {
    int cardlen;
    int cards[7];
    int suitc[4];
    int rankc[13];
} EvalState;

static void _count_suits(EvalState *state)
{
    for (int i = 0; i < state->cardlen; ++i) {
        int suitx = SUITX(state->cards[i]);
        ++state->suitc[suitx];
    }

    printf("Suit counts:\n");
    for (int i = 0; i < ARRAYLEN(state->suitc); ++i) {
        char flushsym = (state->suitc[i] >= 5) ? '*' : ' ';
        printf("\t%c%c: %d\n", flushsym, SuitSym[i], state->suitc[i]);
    }
}

static void _count_ranks(EvalState *state)
{
    for (int i = 0; i < state->cardlen; ++i) {
        int rankx = RANKX(state->cards[i]);
        ++state->rankc[rankx];
    }

    printf("Rank counts:\n");
    for (int i = 0; i < 13; ++i) {
        if (!state->rankc[i])
            continue;
        printf("\t%c: %d\n", RankSym[i], state->rankc[i]);
    }
}

static HandEval _flush(EvalState *state)
{
    for (int i = 0; i < ARRAYLEN(state->suitc); ++i) {
        if (state->suitc[i] >= 5)
            return EVAL_FLUSH;
    }

    return EVAL_NONE;
}

static HandEval _of_a_kind(EvalState *state)
{
    enum {
        KIND_NONE = 0,
        KIND_PAIR = 1,
        KIND_2PAIR = 2 | KIND_PAIR,
        KIND_TRIPS = 4,
        KIND_QUADS = 8,
        KIND_FH = KIND_PAIR | KIND_TRIPS,
    };

    uint8_t ofakind = 0;
    for (int i = 0; i < 13; ++i) {
        switch (state->rankc[i]) {
        case 2:
            ofakind |= ((ofakind & KIND_PAIR) == KIND_PAIR) ? KIND_2PAIR : KIND_PAIR;
            break;
        case 3:
            ofakind |= KIND_TRIPS;
            break;
        case 4:
            ofakind |= KIND_QUADS;
            break;
        default:
            break;
        };
    }

    static const struct {uint8_t kind; HandEval eval;} kind_order[] = {
        {KIND_QUADS, EVAL_QUADS},
        {KIND_FH, EVAL_BOAT},
        {KIND_TRIPS, EVAL_TRIPS},
        {KIND_2PAIR, EVAL_TWOPAIR},
        {KIND_PAIR, EVAL_PAIR},
    };

    for (int i = 0; i < ARRAYLEN(kind_order); ++i) {
        if (HAS_FLAG(ofakind, kind_order[i].kind))
            return kind_order[i].eval;
    }

    return EVAL_NONE;
}

static HandEval _straight(EvalState *state)
{
    int straightc = state->rankc[RANK_A] ? 1 : 0;
    printf("* straightc [%c]: %d (%d)\n", RankSym[RANK_A], straightc, state->rankc[RANK_A]);
    for (int i = 0; i < 13; ++i) {
        if (state->rankc[i]) {
            ++straightc;
            if (straightc == 5)
                break;
        }
        else
            straightc = 0;
        printf("* straightc [%c]: %d (%d)\n", RankSym[i], straightc, state->rankc[i]);
    }
    printf("Has straight: %c\n", straightc == 5 ? 'Y' : 'N');
    return straightc == 5 ? EVAL_STRAIGHT : EVAL_NONE;
}

static void _display(HandEval eval, const char *trying)
{
    printf("Trying %s:\n", trying);
    printf("* Result: %s\n", (eval) ? EVAL_TEXT(eval) : "None");
}

void evaluate(Hand *hand, Board *board)
{
    EvalState state = {
        .cardlen = ARRAYLEN(state.cards),
        .cards = {
            hand->cards[0].id,
            hand->cards[1].id,
            board->cards[0].id,
            board->cards[1].id,
            board->cards[2].id,
            board->cards[3].id,
            board->cards[4].id,
        },
        .suitc = {0, 0, 0, 0},
        .rankc = {0},
    };


    _count_suits(&state);
    _count_ranks(&state);

    _display(_flush(&state), "Flush");
    _display(_of_a_kind(&state), "Of A Kind");
    _display(_straight(&state), "Straight");
}
