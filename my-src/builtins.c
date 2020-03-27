#include "builtins.h"
#include "mpc.h"
#include "lval.h"
#include "lenv.h"

#define LASSERT(args, cond, fmt, ...) \
    if (!(cond)) { \
        lval* err = lval_err(fmt, ##__VA_ARGS__); \
        lval_del(args); \
        return err; \
    }

lval *builtin_head(lenv *e, lval *a) {
    LASSERT(a, a->count == 1, "Function 'head' passed too many arguments. Got %i, expected %i.", a->count, 1);
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'head' passed incorrect types. Got %s, expected %s",
            ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));
    LASSERT(a, a->cell[0]->count != 0, "Function 'head' passed {}!");

    lval *v = lval_take(a, 0);
    while (v->count > 1) { lval_del(lval_pop(v, 1)); }
    return v;
}

lval *builtin_tail(lenv *e, lval *a) {
    LASSERT(a, a->count == 1, "Function 'tail' passed too many arguments. Got %i, expected %i.", a->count, 1);
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'tail' passed incorrect types. Got %s, expected %s",
            ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));
    LASSERT(a, a->cell[0]->count != 0, "Function 'tail' passed {}!");

    lval *v = lval_take(a, 0);
    lval_del(lval_pop(v, 0));
    return v;
}

lval *builtin_list(lenv *e, lval *a) {
    a->type = LVAL_QEXPR;
    return a;
}

lval *builtin_eval(lenv *e, lval *a) {
    LASSERT(a, a->count == 1, "Function 'eval' passed too many arguments. Got %i, expected %i.", a->count, 1);
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'eval' passed incorrect type. Got %s, expected %s",
            ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));

    lval *x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(e, x);
}

lval *builtin_join(lenv *e, lval *a) {
    for (int i = 0; i < a->count; i++) {
        LASSERT(a, a->cell[i]->type == LVAL_QEXPR, "Function 'join' passed incorrect type. Got %s, expected %s",
                ltype_name(a->cell[i]->type), ltype_name(LVAL_QEXPR));
    }

    lval *x = lval_pop(a, 0);
    while (a->count) {
        x = lval_join(x, lval_pop(a, 0));
    }

    lval_del(a);
    return x;
}

lval *builtin_op(lenv *e, lval *a, char *op) {
    /* Ensure all elements are numbers */
    for (int i = 0; i < a->count; i++) {
        LASSERT(a, a->cell[i]->type == LVAL_NUM,
                "Function '%s' passed incorrect type for argument %i. Got %s, expected %s.", op, i,
                ltype_name(a->cell[i]->type), ltype_name(LVAL_NUM));
    }

    /* Pop the first element */
    lval *x = lval_pop(a, 0);

    /* If no arguments and sub then perform unary operation */
    if ((strcmp(op, "-") == 0) && a->count == 0) {
        x->num = -x->num;
    }

    /* While there are still elements remaining */
    while (a->count > 0) {
        /* Pop the next element */
        lval *y = lval_pop(a, 0);

        if (strcmp(op, "+") == 0) { x->num += y->num; }
        if (strcmp(op, "-") == 0) { x->num -= y->num; }
        if (strcmp(op, "*") == 0) { x->num *= y->num; }
        if (strcmp(op, "/") == 0) {
            if (y->num == 0) {
                lval_del(x);
                lval_del(y);
                x = lval_err("Division by zero!");
                break;
            }
            x->num /= y->num;
        }
        lval_del(y);
    }

    lval_del(a);
    return x;
}

lval *builtin_add(lenv *e, lval *a) {
    return builtin_op(e, a, "+");
}

lval *builtin_sub(lenv *e, lval *a) {
    return builtin_op(e, a, "-");
}

lval *builtin_mul(lenv *e, lval *a) {
    return builtin_op(e, a, "*");
}

lval *builtin_div(lenv *e, lval *a) {
    return builtin_op(e, a, "/");
}

lval *builtin_var(lenv *e, lval *a, char *func) {
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'def' passed incorrect type!");

    lval *syms = a->cell[0];
    for (int i = 0; i < syms->count; i++) {
        LASSERT(a, syms->cell[i]->type == LVAL_SYM, "Function 'def' cannot define non-symbol");
    }
    LASSERT(a, syms->count == a->count - 1,
            "Function 'def' cannot take incorrect number of values to symbols. Got %s, expected %s", syms->count,
            a->count - 1)

    for (int i = 0; i < syms->count; i++) {
        // If 'def' define it globally, else define locally
        if (strcmp(func, "def") == 0) {
            lenv_def(e, syms->cell[i], a->cell[i + 1]);
        }
        if (strcmp(func, "=") == 0) {
            lenv_put(e, syms->cell[i], a->cell[i + 1]);
        }
    }
    lval_del(a);
    return lval_sexpr();
}

lval *builtin_def(lenv *e, lval *a) {
    return builtin_var(e, a, "def");
}

lval *builtin_put(lenv *e, lval *a) {
    return builtin_var(e, a, "=");
}

lval *builtin_lambda(lenv *e, lval *a) {
    /* check that both arguments are q-expressions */
    LASSERT(a, a->count == 2, "Function 'lambda' passed incorrect number of arguments. Got %i, expected 2", a->count)
    for (int i = 0; i < 2; i++) {
        LASSERT(a, a->cell[i]->type == LVAL_QEXPR,
                "Function 'lambda' passed incorrect type at argument %i. Got %s, expected %s", i,
                ltype_name(a->cell[i]->type), ltype_name(LVAL_QEXPR))
    }
    // Check the first q-expression contains only symbols
    for (int i = 0; i < a->cell[0]->count; i++) {
        LASSERT(a, a->cell[0]->cell[i]->type == LVAL_SYM, "Cannot define non-symbol. Got %s, expected %s",
                ltype_name(a->cell[0]->cell[i]->type), ltype_name(LVAL_SYM))
    }

    // Pop the first two arguments and pass them to lval_lambda
    lval *formals = lval_pop(a, 0);
    lval *body = lval_pop(a, 0);
    lval_del(a);

    return lval_lambda(formals, body);
}

