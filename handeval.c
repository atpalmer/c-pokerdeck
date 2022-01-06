#include <stdio.h>
#include <stdlib.h>
#include "pokerdeck.h"

#define RANK_EVALBITS(rank, pos)        ((rank) << ((4 - (pos)) * 4))
#define CARD_EVALBITS(card, pos)        (RANK_EVALBITS(RANKX(card), pos))
#define EVALBITS_CARDRANK(bits, pos)    (((bits) >> ((4 - (pos)) * 4)) & 0x000000f)
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

static void _display_counts(EvalState *state)
{
    printf("Sorted Card IDs:");
    for (int i = 0; i < ARRAYLEN(state->cards); ++i) {
        printf(" [%d:%c]", state->cards[i], RANK(state->cards[i]));
    }
    printf("\n");

    printf("Suit counts:\n");
    for (int i = 0; i < ARRAYLEN(state->suitc); ++i) {
        char flushsym = (state->suitc[i] >= 5) ? '*' : ' ';
        printf("\t%c%c: %d\n", flushsym, SuitSym[i], state->suitc[i]);
    }

    printf("Rank counts:\n");
    for (int i = 0; i < 13; ++i) {
        if (!state->rankc[i])
            continue;
        printf("\t%c: %d\n", RankSym[i], state->rankc[i]);
    }
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

    _init(&state);

    _display_counts(&state);

    _display(_straight_flush(&state), "Str. Flush");
    _display(_of_a_kind(&state), "Of A Kind");
    _display(_flush(&state), "Flush");
    _display(_straight(&state), "Straight");
}
