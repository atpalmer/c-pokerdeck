#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pokerdeck.h"

#define NUM_EVALS   (EVALX(EVAL_ROYAL) + 1)

struct winlose {
    unsigned winx: 4;
    unsigned losex: 4;
    unsigned tiebit: 1;
};

static struct winlose get_winlose(Game *this)
{
    if (this->hero.eval > this->villain.eval) {
        struct winlose result = {
            .winx = EVALX(this->hero.eval),
            .losex = EVALX(this->villain.eval),
            .tiebit = 0,
        };
        return result;
    }

    struct winlose result = {
        .winx = EVALX(this->villain.eval),
        .losex = EVALX(this->hero.eval),
        .tiebit = this->hero.eval == this->villain.eval,
    };
    return result;
}

static struct winlose run_sim(void)
{
    Game *game = Game_new_runout();
    struct winlose result = get_winlose(game);
    Game_destroy(game);
    return result;
}

int main(void)
{
    srand(time(NULL));
    static const int ROUNDS = 1000000;

    int win_counts[NUM_EVALS] = {0};
    int loss_counts[NUM_EVALS] = {0};
    int total_counts[NUM_EVALS] = {0};

    for (int i = 0; i < ROUNDS; ++i) {
        struct winlose winlose = run_sim();
        ++total_counts[winlose.winx];
        ++total_counts[winlose.losex];
        if (!winlose.tiebit) {
            ++win_counts[winlose.winx];
            ++loss_counts[winlose.losex];
        }
    }

    printf("%16s (%6s): %6s (%6s); %6s (%6s); %6s (%6s); %6s\n",
        "hand", "freq.", "wins", "win%", "losses", "loss%", "chops", "chop%", "total"
    );
    for (int i = EVALX(EVAL_ROYAL); i >= EVALX(EVAL_NONE); --i) {
        int ties = total_counts[i] - win_counts[i] - loss_counts[i];
        printf("%16s (%5.2f%%): %6d (%5.1f%%); %6d (%5.1f%%); %6d (%5.1f%%); %6d\n",
            EVALX_TEXT(i),
            100.0 * total_counts[i] / ROUNDS,
            win_counts[i],
            100.0 * win_counts[i] / total_counts[i],
            loss_counts[i],
            100.0 * loss_counts[i] / total_counts[i],
            ties,
            100.0 * ties / total_counts[i],
            total_counts[i]);
    }
}
