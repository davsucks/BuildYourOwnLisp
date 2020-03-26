#ifndef LVAL_H
#define LVAL_H

/* Forward Declarations */
struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;

typedef lval *(*lbuiltin)(lenv *, lval *);

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

struct lenv {
    int count;
    char **syms;
    lval **vals;
};

#endif
