#ifndef CONSTRUCTORS_H
#define CONSTRUCTORS_H

#include "lval.h"

lenv *lenv_new();

lval *lval_num(long x);

lval *lval_err(char *fmt, ...);

lval *lval_sym(char *s);

lval *lval_sexpr();

lval *lval_qexpr();

lval *lval_fun(lbuiltin func);

void lval_del(lval *v);

#endif
