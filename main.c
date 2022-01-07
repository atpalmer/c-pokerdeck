#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pokerdeck.h"


/* Deck */

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
    for (int i = 0; i < this->count; ++i) {
        int newpos = rand() % this->count;
        int tmp = this->cards[newpos];
        this->cards[newpos] = this->cards[i];
        this->cards[i] = tmp;
    }
}

int Deck_next(Deck *this)
{
    int id = this->cards[this->count - 1];
    --this->count;
    return id;
}


/* Player */

Player Player_deal(const char *name, Deck *deck)
{
    Player result = {
        .name = name,
        .cards = {
            Deck_next(deck),
            Deck_next(deck),
        },
        .eval = EVAL_NONE,
    };
    return result;
}

void Player_evaluate(Player *p, Board *b)
{
    p->eval = evaluate(p->cards, b->cards);
}

void Player_show_hand(Player *p)
{
    printf("%s:\n", p->name);
    printf("\t%s %s\n", CARD_TEXT(p->cards[0]), CARD_TEXT(p->cards[1]));
}

void Player_show_eval(Player *p)
{
    printf("%10s: %s [", p->name, EVAL_TEXT(p->eval));
    for (int i = 0; i < 5; ++i) {
        int rankx = EVAL_GETRANK(p->eval, i);
        printf("%c", RankSym[rankx]);
    }
    printf("]\n");
}


/* Board */

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

void Board_fill(Board *this, Deck *deck)
{
    for (int i = 0; i < 5; ++i) {
        this->cards[i] = Deck_next(deck);
    }
    this->state = BOARD_RIVER;
}

void Board_deal_streets(Board *board, Deck *deck)
{
    if (!Board_flop(board, deck))
        return;

    printf("Flop:\n");
    printf("\t%s %s %s\n",
        CARD_TEXT(board->cards[0]),
        CARD_TEXT(board->cards[1]),
        CARD_TEXT(board->cards[2]));

    if (!Board_turn(board, deck))
        return;

    printf("Turn:\n");
    printf("\t%s %s %s %s\n",
        CARD_TEXT(board->cards[0]),
        CARD_TEXT(board->cards[1]),
        CARD_TEXT(board->cards[2]),
        CARD_TEXT(board->cards[3]));

    if (!Board_river(board, deck))
        return;

    printf("River:\n");
    printf("\t%s %s %s %s %s\n",
        CARD_TEXT(board->cards[0]),
        CARD_TEXT(board->cards[1]),
        CARD_TEXT(board->cards[2]),
        CARD_TEXT(board->cards[3]),
        CARD_TEXT(board->cards[4]));
}

typedef struct {
    Deck *deck;
    Player hero;
    Player villain;
    Board board;
} Game;

Game *Game_new(void)
{
    Game *new = malloc(sizeof *new);

    new->deck = Deck_new();
    Deck_shuffle(new->deck);

    new->hero = Player_deal("Hero", new->deck);
    new->villain = Player_deal("Villain", new->deck);
    new->board = Board_new();

    return new;
}

void Game_destroy(Game *this)
{
    Deck_cleanup(this->deck);
    free(this);
}

void Game_deal_board(Game *this)
{
    Board_deal_streets(&this->board, this->deck);
}

void Game_fill_board(Game *this)
{
    Board_fill(&this->board, this->deck);
}

#define WINNING_PLAYER(a, b) (((a)->eval == (b)->eval) ? NULL : (((a)->eval > (b)->eval) ? (a) : (b)))

void Game_show_winner(Game *this)
{
    Player *winner = WINNING_PLAYER(&this->hero, &this->villain);
    printf("%10s: %s\n", "Winner", winner ? winner->name : "Chop");
}

void play_round(void)
{
    Game *game = Game_new();

    Player_show_hand(&game->hero);
    Player_show_hand(&game->villain);

    Game_deal_board(game);
    Player_evaluate(&game->hero, &game->board);
    Player_evaluate(&game->villain, &game->board);

    printf("Result:\n");
    Player_show_eval(&game->hero);
    Player_show_eval(&game->villain);

    Game_show_winner(game);

    Game_destroy(game);
}

typedef struct {
    int cardwins[13][13];
    int carddeals[13][13];
} Stats;

#define HIGHCARD(cards) (RANKX((cards)[0]) > RANKX((cards)[1]) ? RANKX((cards)[0]) : RANKX((cards)[1]))
#define LOWCARD(cards) (RANKX((cards)[0]) <= RANKX((cards)[1]) ? RANKX((cards)[0]) : RANKX((cards)[1]))

void Stats_update(Stats *stats, Player *p, Player *winner)
{
    int c1, c2;

    if (SUITX(p->cards[0]) == SUITX(p->cards[1])) {
        c1 = HIGHCARD(p->cards);
        c2 = LOWCARD(p->cards);
    } else {
        c1 = LOWCARD(p->cards);
        c2 = HIGHCARD(p->cards);
    }

    ++stats->carddeals[c1][c2];

    if (p == winner)
        ++stats->cardwins[c1][c2];
}

void play_rounds(int rounds)
{
    Stats stats = {0};

    for (int i = 0; i < rounds; ++i) {
        Game *game = Game_new();
        Game_fill_board(game);
        Player_evaluate(&game->hero, &game->board);
        Player_evaluate(&game->villain, &game->board);
        Player *winner = WINNING_PLAYER(&game->hero, &game->villain);
        Stats_update(&stats, &game->hero, winner);
        Stats_update(&stats, &game->villain, winner);
        Game_destroy(game);
    }

    for (int c1 = RANK_A; c1 >= 0; --c1) {
        for (int c2 = RANK_A; c2 >= 0; --c2) {
            if (!stats.carddeals[c1][c2])
                continue;

            int low, hi, suited;
            if (c1 <= c2) {
                low = c1;
                hi = c2;
                suited = 0;
            } else {
                hi = c1;
                low = c2;
                suited = 1;
            }

            printf("[%c%c%c]: %.01f%% (%d / %d)\n",
                RankSym[hi],
                RankSym[low],
                suited ? 's' : 'o',
                ((double)stats.cardwins[c1][c2] * 100.0) / (double)stats.carddeals[c1][c2],
                stats.cardwins[c1][c2],
                stats.carddeals[c1][c2]);
        }
    }
}

int main(void)
{
    srand(time(NULL));
    play_rounds(1000000);
}
