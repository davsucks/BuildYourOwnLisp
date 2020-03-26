#ifndef LVAL_H
#define LVAL_H

#include "builtins.h"

enum {
    LVAL_NUM,
    LVAL_ERR,
    LVAL_SYM,
    LVAL_FUN,
    LVAL_SEXPR,
    LVAL_QEXPR
};

struct lval {
    int type;

    long num;
    char *err;
    char *sym;
    lbuiltin fun;

    int count;
    struct lval **cell;
};

// Utils
char *ltype_name(int t);

void lval_print(lval *v);

void lval_println(lval *v);

// Constructors
lval *lval_num(long x);

lval *lval_err(char *fmt, ...);

lval *lval_sym(char *s);

lval *lval_sexpr();

lval *lval_qexpr();

lval *lval_fun(lbuiltin func);

// Operations
lval *lval_add(lval *v, lval *x);

lval *lval_copy(lval *v);

lval *lval_pop(lval *v, int i);

lval *lval_take(lval *v, int i);

lval *lval_join(lval *x, lval *y);

void lval_del(lval *v);

lval *lval_eval(lenv *e, lval *v);

lval *lval_eval_sexpr(lenv *e, lval *v);

#endif
