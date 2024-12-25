#ifndef LAM_H
#define LAM_H

#include "base.h"

typedef enum Kind {
    VAR = 1, // variable
    LAM, // lambda definition
    APP, // lambda application
} Kind;

#define SLOTS_MAX ((1u << 30) - 1)

#define NULL_REF ((Ref){.index = 0, .kind = 0})

typedef struct Ref {
    // for variables, this acts as a debruijn index
    // for lambdas and applications, this acts as an
    // index to more exprs
    u32 index : 30;
    Kind kind : 2;
} Ref;
static_assert(sizeof(Ref) == sizeof(u32));

// typedef union Expr {
//     Ref lam; // body of lambda
//     struct {
//         Ref lam; // lambda to apply
//         Ref exp; // expression to apply lambda to
//     } app;
// } Expr;

typedef struct Lam {
    Ref lam;
} Lam;
typedef struct App {
    Ref lam;
    Ref exp;
} App;

typedef struct Pool {
    union {
        Ref* refs;    
        u32* slots;
    };
    u32 len;
    u32 cap;

    u32 slots_free;
} Pool;

extern Pool pool;

void init();
Lam* lam(Ref x);
App* app(Ref x);
Ref pool_condense(Ref head);
Ref new(u32 kind);
void expire(Ref x);

void print(Ref head);
void print_debruijn(Ref head);

Ref parse(char* s);

Ref reduce(Ref tree);
extern bool did_beta;

#endif