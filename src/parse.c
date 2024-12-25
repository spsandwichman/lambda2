#include "lam.h"

const char* lambda = "λ";

struct {
    u32 cursor;
    char* s;
    
    struct {
        char* at;
        u32 len;
        u32 cap;
    } params;
} p;
#define at() p.s[p.cursor]
#define advance() ++p.cursor

static void skip_whitespace() {
    while (at() != '\0') {
        switch (at()) {
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            advance();
            break;
        default:
            return;
        }
    }
}

static __attribute__((noreturn)) void parse_error(char* s) {
    printf("error: %s at col %d\n", s, p.cursor);
    exit(1);
}

static void push_var(char v) {
    if (p.params.len == p.params.cap) {
        p.params.cap *= 2;
        p.params.at = realloc(p.params.at, sizeof(p.params.at[0]) * p.params.cap);
    }
    
    p.params.at[p.params.len++] = v;
}

static void pop_var() {
    p.params.len--;
}

static bool is_letter(char c) {
    return
        ('a' <= c && c <= 'z') ||
        ('A' <= c && c <= 'Z');
}

static Ref parse_expr();

static Ref parse_lambda() {
    Ref l = new(LAM);
        // printf("|%s\n", p.s + p.cursor);

    if (!is_letter(at())) {
        parse_error("expected variable");
    }

    push_var(at());
    advance();

    if (at() == '.') {
        advance();
        lam(l)->lam = parse_expr();
    } else {
        lam(l)->lam = parse_lambda();
    }

    pop_var();
    return l;
}


static Ref parse_term() {
    skip_whitespace();


    // lambda
    if (at() == lambda[0] || at() == '\\') {
        advance();
        if (at() == lambda[1]) advance();
        return parse_lambda();
    }
    // expr
    if (at() == '(') {
        advance();
        Ref in = parse_expr();
        skip_whitespace();
        if (at() != ')') {
            parse_error("expected )");  
        }
        advance();
        return in;
    }

    // var
    if (is_letter(at())) {
        // get debruijn index
        for(i64 index = 1; (i64)p.params.len - index >= 0; ++index) {
            if (p.params.at[p.params.len - index] == at()) {
                Ref var = new(VAR);
                var.index = index;
                advance();
                return var;
            }
        }
        parse_error("undefined variable");
    }

    parse_error("expected term");
}

static bool is_term_start(char c) {
    return
        is_letter(c) ||
        c == '(' ||
        c == lambda[0] ||
        c == '\\';
}

static Ref parse_expr() {
    Ref left = parse_term();
    skip_whitespace();
    while (is_term_start(at())) {
        
        Ref right = parse_term();

        Ref a = new(APP);

        app(a)->lam = left;
        app(a)->exp = right;

        left = a;

        skip_whitespace();
    }

    return left;
}

Ref parse(char* s) {
    p.params.len = 0;
    p.params.cap = 128;
    p.params.at = malloc(sizeof(p.params.at[0]) * p.params.cap);

    p.s = s;
    p.cursor = 0;

    return parse_expr();
}