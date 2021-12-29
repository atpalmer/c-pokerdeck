#include <stdio.h>
#include <stdlib.h>
#include "pokerdeck.h"

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
        switch (rankc[i]) {
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

    int straightc = rankc[RANK_A] ? 1 : 0;
    printf("* straightc [%c]: %d (%d)\n", RankSym[RANK_A], straightc, rankc[RANK_A]);
    for (int i = 0; i < 13; ++i) {
        if (rankc[i]) {
            ++straightc;
            if (straightc == 5)
                break;
        }
        else
            straightc = 0;
        printf("* straightc [%c]: %d (%d)\n", RankSym[i], straightc, rankc[i]);
    }
    printf("Has straight: %c\n", straightc == 5 ? 'Y' : 'N');
}
