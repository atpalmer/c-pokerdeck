#include <stdio.h>
#include <stdlib.h>
#include "pokerdeck.h"

typedef struct {
    int cardlen;
    int cards[7];
    int suitc[4];
    int rankc[13];
} EvalState;

static int _flush(EvalState *state)
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

    for (int i = 0; i < ARRAYLEN(state->suitc); ++i) {
        if (state->suitc[i] >= 5)
            return 1;
    }

    return 0;
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

static void _of_a_kind(EvalState *state)
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

    struct {uint8_t kind; const char *text;} kind_order[] = {
        {KIND_QUADS, "Quads"},
        {KIND_FH, "Full House"},
        {KIND_TRIPS, "Trips"},
        {KIND_2PAIR, "Two Pair"},
        {KIND_PAIR, "Pair"},
        {KIND_NONE, NULL},
    };

    for (int i = 0; i < 5; ++i) {
        char sym = ((kind_order[i].kind & ofakind) == kind_order[i].kind) ? 'Y' : 'N';
        printf("%s: %c\n", kind_order[i].text, sym);
    }
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

    _flush(&state);

    _count_ranks(&state);

    _of_a_kind(&state);

    int straightc = state.rankc[RANK_A] ? 1 : 0;
    printf("* straightc [%c]: %d (%d)\n", RankSym[RANK_A], straightc, state.rankc[RANK_A]);
    for (int i = 0; i < 13; ++i) {
        if (state.rankc[i]) {
            ++straightc;
            if (straightc == 5)
                break;
        }
        else
            straightc = 0;
        printf("* straightc [%c]: %d (%d)\n", RankSym[i], straightc, state.rankc[i]);
    }
    printf("Has straight: %c\n", straightc == 5 ? 'Y' : 'N');
}
