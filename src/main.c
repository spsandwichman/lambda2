#include "lam.h"
#include "lib.h"

int main() {
    init();

    Ref l = 
        parse("(λx.xxx) (λx.xxx)");
        // parse("(λxy.x(x(xy))) (λxy.x(x(xy))) (λxy.x(x(xy)))");
        // parse("(λxy.x(x(xy))) λxy.x(x(xy))");
        // parse(MUL N1 N2);
        // parse("λxy.(λzw.z (z w)) x y");

    // print_debruijn(l);
    // putchar('\n');
    print(l);
    putchar('\n');

    do {
        l = reduce(l);
        // l = pool_condense(l);
        
        // print_debruijn(l);
        // print(l);
        // putchar('\n');

        float percent_free = pool.slots_free/(float)pool.len;
        if (percent_free >= .333) {
            l = pool_condense(l);
        }
        printf("%p %d/%d (%f%% free)\n", pool.refs, pool.len, pool.cap, percent_free * 100.0);
    } while (did_beta);

    print(l);
    putchar('\n');
}
