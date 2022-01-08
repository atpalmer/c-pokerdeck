#include <stdlib.h>
#include "pokerdeck.h"

#define RANK_EVALBITS(rank, pos)        ((rank) << ((4 - (pos)) * 4))
#define CARD_EVALBITS(card, pos)        (RANK_EVALBITS(RANKX(card), pos))
#define RANK_BITS(r1, r2, r3, r4, r5)   ((r1) << (4 * 4) | (r2) << (3 * 4) | (r3) << (2 * 4) | (r4) << (1 * 4) | (r5) << (0 * 4))

typedef struct {
    int cards[7];
    int suitc[4];
    int rankc[13];
    uint8_t ranksuits[13];
} EvalState;

static void _init(EvalState *state)
{
    /* sort */
    for (int i = 0; i < ARRAYLEN(state->cards) - 1; ++i) {
        int highx = i;
        for (int j = i + 1; j < ARRAYLEN(state->cards); ++j) {
            if (RANKX(state->cards[j]) > RANKX(state->cards[highx]))
                highx = j;
        }
        if (highx == i)
            continue;
        int tmp = state->cards[highx];
        state->cards[highx] = state->cards[i];
        state->cards[i] = tmp;
    }

    /* count suits */
    for (int i = 0; i < ARRAYLEN(state->cards); ++i) {
        int suitx = SUITX(state->cards[i]);
        ++state->suitc[suitx];
    }

    /* count ranks */
    for (int i = 0; i < ARRAYLEN(state->cards); ++i) {
        int rankx = RANKX(state->cards[i]);
        ++state->rankc[rankx];
    }

    /* count ranksuits */
    for (int i = 0; i < ARRAYLEN(state->cards); ++i) {
        int rankx = RANKX(state->cards[i]);
        int suitx = SUITX(state->cards[i]);
        uint8_t suitbits = 1 << suitx;
        state->ranksuits[rankx] = suitbits;
    }
}

static HandEval _flush(EvalState *state)
{
    for (int suitx = 0; suitx < ARRAYLEN(state->suitc); ++suitx) {
        if (state->suitc[suitx] >= 5) {
            uint32_t evalbits = 0;
            for (int handpos = 0, i = 0; handpos < 5 && i < ARRAYLEN(state->cards); ++i) {
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

    struct kind_entry {
        uint8_t kind;
        HandEval eval;
        int highcards;
    };

    struct kind_entry quadsentry = {KIND_QUADS, EVAL_QUADS, 1};
    struct kind_entry boatentry = {KIND_FH, EVAL_BOAT, 0};
    struct kind_entry tripsentry = {KIND_TRIPS, EVAL_TRIPS, 2};
    struct kind_entry twopairentry = {KIND_2PAIR, EVAL_TWOPAIR, 1};
    struct kind_entry pairentry = {KIND_PAIR, EVAL_PAIR, 3};

    struct kind_entry *kind_order[] = {
        &quadsentry,
        &boatentry,
        &tripsentry,
        &twopairentry,
        &pairentry,
    };

    uint8_t ofakind = 0;
    for (int i = 12; i >= 0; --i) {
        switch (state->rankc[i]) {
        case 1:
            for (int k = 0; k < ARRAYLEN(kind_order); ++k) {
                struct kind_entry *entry = kind_order[k];
                if (entry->highcards) {
                    --entry->highcards;
                    entry->eval |= (i << (entry->highcards * 4));
                }
            }
            break;
        case 2:
            if (HAS_FLAG(ofakind, KIND_PAIR)) {
                if (HAS_FLAG(ofakind, KIND_2PAIR))
                    continue;
                ofakind |= KIND_2PAIR;
                twopairentry.eval |= RANK_BITS(0, 0, i, i, 0);
            }
            else {
                if (HAS_FLAG(ofakind, KIND_PAIR))
                    continue;
                ofakind |= KIND_PAIR;
                pairentry.eval |= RANK_BITS(i, i, 0, 0, 0);
                twopairentry.eval |= RANK_BITS(i, i, 0, 0, 0);
                boatentry.eval |= RANK_BITS(0, 0, 0, i, i);
            }
            break;
        case 3:
            if (HAS_FLAG(ofakind, KIND_TRIPS))
                continue;
            ofakind |= KIND_TRIPS;
            tripsentry.eval |= RANK_BITS(i, i, i, 0, 0);
            boatentry.eval |= RANK_BITS(i, i, i, 0, 0);
            break;
        case 4:
            if (HAS_FLAG(ofakind, KIND_QUADS))
                continue;
            ofakind |= KIND_QUADS;
            quadsentry.eval |= RANK_BITS(i, i, i, i, 0);
            break;
        default:
            break;
        };
    }

    for (int i = 0; i < ARRAYLEN(kind_order); ++i) {
        if (HAS_FLAG(ofakind, kind_order[i]->kind))
            return kind_order[i]->eval;
    }

    return EVAL_NONE;
}

static HandEval _straight_flush(EvalState *state)
{
    uint32_t evalbits = 0;
    int straightc = 0;
    uint8_t suitmask = 0;
    for (int i = RANK_A; i >= 0; --i) {
        suitmask &= state->ranksuits[i];
        if (suitmask) {
            evalbits |= RANK_EVALBITS(i, straightc);
            ++straightc;
            if (straightc == 5)
                goto straightflush;
        }
        else if (state->ranksuits[i]) {
            evalbits = RANK_EVALBITS(i, 0);
            suitmask = state->ranksuits[i];
            straightc = 1;
        }
        else {
            evalbits = 0;
            suitmask = 0;
            straightc = 0;
        }
    }
    if (suitmask & state->ranksuits[RANK_A]) {
        evalbits |= RANK_A;
        ++straightc;
        if (straightc == 5)
            goto straightflush;
    }

    return EVAL_NONE;

straightflush:
    return (evalbits == 0xcba98) ? EVAL_ROYAL : (EVAL_STFLUSH | evalbits);
}

static HandEval _straight(EvalState *state)
{
    uint32_t evalbits = 0;
    int straightc = 0;
    for (int i = RANK_A; i >= 0; --i) {
        if (state->rankc[i]) {
            evalbits |= RANK_EVALBITS(i, straightc);
            ++straightc;
            if (straightc == 5)
                goto straight;
        }
        else {
            evalbits = 0;
            straightc = 0;
        }
    }
    if (state->rankc[RANK_A]) {
        evalbits |= RANK_A;
        ++straightc;
        if (straightc == 5)
            goto straight;
    }

    return EVAL_NONE;

straight:
    return EVAL_STRAIGHT | evalbits;
}

static HandEval _high_card(EvalState *state)
{
    uint32_t evalbits = 0x0;
    for (int i = 0; i < 5; ++i)
        evalbits |= CARD_EVALBITS(state->cards[i], i);
    return EVAL_NONE | evalbits;
}

HandEval evaluate(int *handcards, int *boardcards)
{
    EvalState state = {
        .cards = {
            handcards[0],
            handcards[1],
            boardcards[0],
            boardcards[1],
            boardcards[2],
            boardcards[3],
            boardcards[4],
        },
        .suitc = {0, 0, 0, 0},
        .rankc = {0},
        .ranksuits = {0},
    };

    _init(&state);

    HandEval stfl = _straight_flush(&state);
    if (stfl)
        return stfl;

    HandEval ofakind = _of_a_kind(&state);
    if (ofakind > EVAL_FLUSH)
        return ofakind;

    HandEval flush = _flush(&state);
    if (flush)
        return flush;

    HandEval straight = _straight(&state);
    if (straight)
        return straight;

    if (ofakind)
        return ofakind;

    return _high_card(&state);
}
