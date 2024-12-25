#include "lam.h"

// manage all the lambda expressions in a big array of u32.

Pool pool;

void init() {
    pool.len = 0;
    pool.cap = 2048;
    pool.slots_free = 0;
    pool.slots = malloc(sizeof(*pool.slots) * pool.cap);
}

Lam* lam(Ref x) {
    return (Lam*) &pool.slots[x.index];

}
App* app(Ref x) {
    return (App*) &pool.slots[x.index];
}

// free this expression
void expire(Ref x) {
    // if (x.kind == VAR) return;
    assert(x.kind != VAR);
    pool.slots[x.index] = 0;
    pool.slots_free += 1;
    if (x.kind == APP) {
        pool.slots[x.index + 1] = 0;
        pool.slots_free += 1;
    }
}

// condense the expression pool and translate tree head
Ref pool_condense(Ref head) {
    // create a mapping between the old ref indexes and new ref indexes
    u32* new_indexes = malloc(sizeof(u32) * pool.len);

    u32 top = 0;
    for (u32 cursor = 0; cursor < pool.len; ++cursor) {
        // if an expression lives here,
        if (pool.slots[cursor] != 0) {
            // copy into its new home
            pool.slots[top] = pool.slots[cursor];
            // tell the next pass that this expression now lives somewhere else
            new_indexes[cursor] = top;

            ++top;
        }
    }

    pool.len = top;

    if (head.kind != VAR) {
        head.index = new_indexes[head.index];
    }

    for (u32 i = 0; i < pool.len; ++i) {
        // map old index to new index
        Ref r = pool.refs[i];
        if (r.kind != VAR) {
            r.index = new_indexes[r.index];
        }
        pool.refs[i] = r;
    }

    free(new_indexes);

    pool.slots_free = 0;
    return head;
}

Ref new(u32 kind) {
    if (kind == VAR) {
        return (Ref){.kind = VAR};
    }
    u32 slots = kind == APP ? 2 : 1;

    if (pool.len + slots > pool.cap) {
        pool.cap *= 2;
        pool.slots = realloc(pool.slots, sizeof(u32) * pool.cap);
    }

    Ref r;
    r.kind = kind;
    r.index = pool.len;

    pool.len += slots;

    if (pool.len > SLOTS_MAX) {
        printf("exceeded %d nodes", SLOTS_MAX);
        exit(1);
    }

    return r;
}