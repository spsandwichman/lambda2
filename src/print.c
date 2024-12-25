#include "lam.h"

static char varnames[] = {
    'x', 'y', 'z', 'w',
    'u', 'v', 'm', 'n',
    'i', 'j', 'k', 

    'a', 'b', 'c', 'd', 'e',
    'f', 'g', 'h', 'l', 'o',
    'p', 'q', 'r', 's', 't'
};


static void print_internal(Ref t, u32 db_offset, bool outer) {
    switch (t.kind) {
    case VAR:
        printf("%c", varnames[(db_offset - t.index) % 26]);
        break;
    case LAM:
        if (!outer) printf("(");
        printf("λ");
        while (t.kind == LAM) {
            putchar(varnames[(db_offset) % 26]);
            t = lam(t)->lam;
            ++db_offset;
        }
        putchar('.');
        print_internal(t, db_offset, outer);
        if (!outer) printf(")");
        break;
    case APP:
        print_internal(app(t)->lam, db_offset, false);
        if (app(t)->exp.kind != VAR) {
            printf(" ");
        }
        if (app(t)->exp.kind == APP) {
            printf("(");
            print_internal(app(t)->exp, db_offset, true);
            printf(")");
        } else {
            print_internal(app(t)->exp, db_offset, outer);
        }
        break;
    default:
        unreachable;
    }
}

static void print_debruijn_internal(Ref t, bool outer) {
    switch (t.kind) {
    case VAR:
        printf("%d", t.index);
        break;
    case LAM:
        if (!outer) printf("(");
        while (t.kind == LAM) {
            printf("λ");
            t = lam(t)->lam;
        }
        print_debruijn_internal(t, outer);
        if (!outer) printf(")");
        break;
    case APP:
        print_debruijn_internal(app(t)->lam, false);
        if (app(t)->lam.kind != VAR || (app(t)->lam.kind == VAR && app(t)->exp.kind == VAR)) {
            printf(" ");
        }
        if (app(t)->exp.kind == APP) {
            printf("(");
            print_debruijn_internal(app(t)->exp, true);
            printf(")");
        } else {
            print_debruijn_internal(app(t)->exp, outer);
        }
        break;
    default:
        unreachable;
    }
}

void print(Ref head) {
    print_internal(head, 0, true);
}

void print_debruijn(Ref head) {
    print_debruijn_internal(head, true);
}