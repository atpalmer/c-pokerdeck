#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pokerdeck.h"

#define HIGHCARD(cards) (RANKX((cards)[0]) > RANKX((cards)[1]) ? RANKX((cards)[0]) : RANKX((cards)[1]))
#define LOWCARD(cards) (RANKX((cards)[0]) <= RANKX((cards)[1]) ? RANKX((cards)[0]) : RANKX((cards)[1]))

typedef struct {
    int cardwins[13][13];
    int carddeals[13][13];
} Stats;

void Stats_update(Stats *stats, Player *p, Player *winner)
{
    int c1, c2;

    if (SUITX(p->cards[0]) == SUITX(p->cards[1])) {
        /* encode suited hands as [high][low] */
        c1 = HIGHCARD(p->cards);
        c2 = LOWCARD(p->cards);
    } else {
        /* encode offsuit hands as [low][high] */
        c1 = LOWCARD(p->cards);
        c2 = HIGHCARD(p->cards);
    }

    ++stats->carddeals[c1][c2];

    if (p == winner)
        ++stats->cardwins[c1][c2];
}

void Stats_update_from_Game(Stats *stats, Game *game)
{
    Player *winner = WINNING_PLAYER(&game->hero, &game->villain);
    Stats_update(stats, &game->hero, winner);
    Stats_update(stats, &game->villain, winner);
}

typedef struct {
    char sym[4];
    double win_pct;
} Summary;

void Summary_sort(Summary **first, Summary **last)
{
    for (Summary **o = first; o <= last - 1; ++o) {
        Summary **max = o;
        for (Summary **i = o + 1; i <= last; ++i) {
            if ((*i)->win_pct > (*max)->win_pct)
                max = i;
        }
        if (max != o) {
            Summary *tmp = *max;
            *max = *o;
            *o = tmp;
        }
    }
}

Summary **Summary_create(Stats *stats)
{
    Summary **result = malloc(sizeof(*result) * 13 * 13);

    for (int c1 = RANK_A; c1 >= 0; --c1) {
        for (int c2 = RANK_A; c2 >= 0; --c2) {
            Summary *s = malloc(sizeof *s);

            if (c1 <= c2) {
                /* offsuit hands are encoded [low][high] */
                s->sym[0] = RankSym[c2];
                s->sym[1] = RankSym[c1];
                s->sym[2] = 'o';
                s->sym[3] = '\0';
            } else {
                /* suited hands are encoded [high][low] */
                s->sym[0] = RankSym[c1];
                s->sym[1] = RankSym[c2];
                s->sym[2] = 's';
                s->sym[3] = '\0';
            }

            s->win_pct = (double)stats->cardwins[c1][c2] / (double)stats->carddeals[c1][c2];

            result[(13 * c1) + c2] = s;
        }
    }

    Summary_sort(&result[0], &result[(13 * 13) - 1]);

    return result;
}

void Summary_destroy(Summary **this)
{
    for (int i = 0; i < 13 * 13; ++i)
        free(this[i]);
    free(this);
}

void top_hands(int rounds)
{
    Stats *stats = calloc(sizeof *stats, 1);

    for (int i = 0; i < rounds; ++i) {
        Game *game = Game_new_runout();
        Stats_update_from_Game(stats, game);
        Game_destroy(game);
    }

    Summary **summary = Summary_create(stats);

    printf("Top starting hands for %d simulations:\n", rounds);
    for (int i = 0; i < 13 * 13; ++i) {
        Summary *s = summary[i];
        printf("%d) [%s]: %.01f%%\n", i + 1, s->sym, s->win_pct * 100);
    }

    Summary_destroy(summary);

    free(stats);
}

int main(void)
{
    srand(time(NULL));
    top_hands(1000000);
}
