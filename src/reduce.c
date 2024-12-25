#include "lam.h"

void destroy(Ref t) {
    switch (t.kind) {
    case LAM:
        destroy(lam(t)->lam);
        expire(t);
        break;
    case APP:
        destroy(app(t)->lam);
        destroy(app(t)->exp);
        expire(t);
        break;
    case VAR:
        // dont expire, since it doesnt actually take node space.
        // VARs live completely in their references
        break;
    default:
        unreachable;
    }
}

Ref inc_free(Ref input, isize depth, isize inc) {
    switch (input.kind) {
    case LAM:
        Ref body = inc_free(lam(input)->lam, depth+1, inc);
        lam(input)->lam = body;
        break;
    case APP:
        Ref l = inc_free(app(input)->lam, depth, inc);
        Ref e = inc_free(app(input)->exp, depth, inc);
        app(input)->lam = l;
        app(input)->exp = e;
        break;
    case VAR:
        if (input.index >= depth) {
            input.index += inc;
        }
        break;
    default:
        unreachable;
    }
    return input;
}

Ref duplicate_inc_free(Ref input, isize depth, isize inc) {
    Ref x;

    switch (input.kind) {
    case LAM:
        x = new(LAM);
        Ref body = duplicate_inc_free(lam(input)->lam, depth + 1, inc);
        lam(x)->lam = body;
        break;
    case APP:
        x = new(APP);
        Ref l = duplicate_inc_free(app(input)->lam, depth, inc);
        Ref e = duplicate_inc_free(app(input)->exp, depth, inc);
        app(x)->lam = l;
        app(x)->exp = e;
        break;
    case VAR:
        x = new(VAR);
        x.index = input.index;
        if (x.index >= depth) {
            x.index += inc;
        }
        break;
    default:
        unreachable;
    }
    return x;
}

static usize num_bound = 0;
static usize inc_offset = 0;

Ref traverse(Ref body, Ref input, isize depth) {
    switch (body.kind) {
    case LAM:
        Ref new_body = traverse(lam(body)->lam, input, depth + 1);
        lam(body)->lam = new_body;
        break;
    case APP:
        Ref new_lam = traverse(app(body)->lam, input, depth);
        app(body)->lam = new_lam;
        Ref new_exp = traverse(app(body)->exp, input, depth);
        app(body)->exp = new_exp;
        break;
    case VAR:
        if (body.index == depth) {
            num_bound++;
            // if (num_bound == 1) {
            //     input = inc_free(input, 1, depth - 1);
            //     inc_offset = depth - 1;
            //     return input;
            // }
            // return duplicate_inc_free(input, 1, (depth - 1) - inc_offset);
            return duplicate_inc_free(input, 1, depth - 1);
        }
        if (body.index > depth) {
            body.index--;
        }
        break;
    default:
        unreachable;
    }
    return body;
}

bool can_reduce(Ref t) {
    return t.kind == APP && app(t)->lam.kind == LAM;
}

Ref beta(Ref t) {
    assert(can_reduce(t));

    num_bound = 0;
    inc_offset = 0;

    // (λ M) N
    Ref M = lam(app(t)->lam)->lam;
    Ref N = app(t)->exp;

    M = traverse(M, N, 1);

    // if (num_bound == 0) 
    destroy(N);
    expire(app(t)->lam);
    expire(t);

    return M;
}

#define as_u32(r) (*(u32*)(&r))

bool did_beta = false;

// returns NULL_REF if no sub happened
Ref reduce_outer_left(Ref tree) {
    switch (tree.kind) {
    case VAR:
        return tree;
    case LAM:
        Ref sub = reduce_outer_left(lam(tree)->lam);
        lam(tree)->lam = sub;
        return tree;
    case APP:
        if (app(tree)->lam.kind == LAM) {
            did_beta = true;
            return beta(tree);
        }
        Ref left = reduce_outer_left(app(tree)->lam);
        app(tree)->lam = left;
        if (did_beta) return tree;
        Ref right = reduce_outer_left(app(tree)->exp);
        app(tree)->exp = right;
        return tree;
    default:
        unreachable;
    }
}

Ref reduce(Ref tree) {
    did_beta = false;
    return reduce_outer_left(tree);
}