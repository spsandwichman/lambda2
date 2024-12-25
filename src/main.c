#include "lam.h"
#include "lib.h"

int main() {
    init();

    // char* text = MUL N100 N10;
    // char* text = IFTHEN FALSE N2 N3;
    char* text = "(\\x.xxx) (\\x.xxx)";

    printf("%s\n", text);

    Ref l = parse(text);

    // print(l);
    // printf("\n");
    print_debruijn(l);
    printf("\n");
    usize steps = 0;
    do {
        l = reduce(l);
        // l = pool_condense(l);
        
        // print(l);
        // printf("\n");
        // print_debruijn(l);
        // printf("\n");

        double percent_free = (pool.slots_free*1.)/(pool.cap);
        printf("% 5d/% 5d (% 2f%% free)\n", pool.len - pool.slots_free, pool.cap, percent_free*100);
        if (percent_free >= .50) {
            l = pool_condense(l);
        }
        steps++;
    } while (did_beta);

    print(l);
    printf("\n");

}
