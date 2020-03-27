#ifndef OPERATIONS_H
#define OPERATIONS_H

// Forward declarations
struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;

typedef lval *(*lbuiltin)(lenv *, lval *);

lval *builtin_list(lenv *e, lval *a);

lval *builtin_head(lenv *e, lval *a);

lval *builtin_tail(lenv *e, lval *a);

lval *builtin_eval(lenv *e, lval *a);

lval *builtin_join(lenv *e, lval *a);

lval *builtin_op(lenv *e, lval *a, char *op);

lval *builtin_add(lenv *e, lval *a);

lval *builtin_sub(lenv *e, lval *a);

lval *builtin_mul(lenv *e, lval *a);

lval *builtin_div(lenv *e, lval *a);

lval *builtin_def(lenv *e, lval *a);

lval *builtin_put(lenv *e, lval *a);

lval *builtin_lambda(lenv *e, lval *a);

#endif
