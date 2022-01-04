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

#define RANK_EVALBITS(rank, pos)        ((rank) << ((4 - (pos)) * 4))
#define CARD_EVALBITS(card, pos)        (RANK_EVALBITS(RANKX(card), pos))
#define EVALBITS_CARDRANK(bits, pos)    (((bits) >> ((4 - (pos)) * 4)) & 0x000000f)

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

static void _sort_cards(EvalState *state)
{
    for (int i = 0; i < state->cardlen - 1; ++i) {
        int highx = i;
        for (int j = i + 1; j < state->cardlen; ++j) {
            if (RANKX(state->cards[j]) > RANKX(state->cards[highx]))
                highx = j;
        }
        if (highx == i)
            continue;
        int tmp = state->cards[highx];
        state->cards[highx] = state->cards[i];
        state->cards[i] = tmp;
    }

    printf("Sorted Card IDs:");
    for (int i = 0; i < state->cardlen; ++i) {
        printf(" [%d:%c]", state->cards[i], RANK(state->cards[i]));
    }
    printf("\n");
}

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
    for (int suitx = 0; suitx < ARRAYLEN(state->suitc); ++suitx) {
        if (state->suitc[suitx] >= 5) {
            uint32_t evalbits = 0;
            for (int handpos = 0, i = 0; handpos < 5 && i < state->cardlen; ++i) {
                if (suitx == SUITX(state->cards[i])) {
                    evalbits |= CARD_EVALBITS(state->cards[i], handpos++);
                }
            }

            return EVAL_FLUSH | evalbits;
        }
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
            ofakind |= HAS_FLAG(ofakind, KIND_PAIR) ? KIND_2PAIR : KIND_PAIR;
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
    int straightc = 0;
    uint32_t evalbits = 0;
    int bitpos = 0;
    for (int i = 12; i >= 0; --i) {
        if (state->rankc[i]) {
            evalbits |= RANK_EVALBITS(i, bitpos);
            ++bitpos;
            ++straightc;
            if (straightc == 5)
                goto straight;
        }
        else {
            straightc = 0;
            evalbits = 0;
            bitpos = 0;
        }
    }
    if (state->rankc[RANK_A]) {
        evalbits |= RANK_A;
        ++straightc;
    }
    if (straightc == 5)
        goto straight;

    return EVAL_NONE;

straight:
    return EVAL_STRAIGHT | evalbits;
}

static void _display(HandEval eval, const char *trying)
{
    printf("Trying... %10s: ", trying);

    if (eval) {
        printf("%s [bits: 0x%x];", EVAL_TEXT(eval), eval);

        printf(" hand ranks: ");
        for (int i = 0; i < 5; ++i) {
            int rankx = EVALBITS_CARDRANK(eval, i);
            printf(" [%c]", RankSym[rankx]);
        }
        printf("\n");
    }
    else
        printf("None\n");
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

    _sort_cards(&state);
    _count_suits(&state);
    _count_ranks(&state);

    _display(_of_a_kind(&state), "Of A Kind");
    _display(_flush(&state), "Flush");
    _display(_straight(&state), "Straight");
}
