#include "lam.h"

int main() {
    init();


    Ref l = 
        parse("(λx.xxx) (λx.xxx)");
        // parse("(λxy.x(x(xy))) (λxy.x(x(xy))) (λxy.x(x(xy)))");
        // parse("(λxy.x(x(xy))) λxy.x(x(xy))");

    print(l);
    putchar('\n');

    do {
        l = reduce(l);
        // l = pool_condense(l);
        // print(l);
        // putchar('\n');
        // printf("%p %d/%d (%f%% free)\n", pool.refs, pool.len, pool.cap, pool.slots_free/(float)pool.len * 100.0);
    } while (did_beta);
}
