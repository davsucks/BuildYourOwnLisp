#ifndef LENV_H
#define LENV_H

#include "lval.h"

struct lenv {
    lenv *parent;
    int count;
    char **syms;
    lval **vals;
};

lenv *lenv_new();

lenv *lenv_copy(lenv *e);

lval *lenv_get(lenv *e, lval *k);

void lenv_put(lenv *e, lval *k, lval *v);

void lenv_def(lenv *e, lval *k, lval *v);

void lenv_add_builtins(lenv *e);

void lenv_add_builtin(lenv *e, char *name, lbuiltin func);

void lenv_del(lenv *e);

#endif
