#ifndef PARSING_H
#define PARSING_H

#include "lval.h"
#include "mpc.h"

lval *lval_add(lval *v, lval *x);

lval* lval_read(mpc_ast_t* t);

#endif
