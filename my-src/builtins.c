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

#define LASSERT_TYPE(func, args, index, expected_type) \
    LASSERT(args, args->cell[index]->type == expected_type, \
    "Function '%s' passed incorrect type for argument %i. Got %s, expected %s.", \
    func, index, ltype_name(args->cell[index]->type), ltype_name(expected_type))

#define LASSERT_NUM(func, args, num) \
    LASSERT(args, args->count == num, \
    "Function '%s' passed incorrect number of arguments. Got %i, expected %i.", \
    func, args->count, num)

#define LASSERT_NOT_EMPTY(func, args, index) \
    LASSERT(args, args->cell[index]->count != 0, \
    "Function '%s' passed () for argument %i.", func, index)

lval *builtin_head(lenv *e, lval *a) {
    LASSERT_NUM("head", a, 1)
    LASSERT_TYPE("head", a, 0, LVAL_QEXPR)
    LASSERT_NOT_EMPTY("head", a, 0)

    lval *v = lval_take(a, 0);
    while (v->count > 1) { lval_del(lval_pop(v, 1)); }
    return v;
}

lval *builtin_tail(lenv *e, lval *a) {
    LASSERT_NUM("tail", a, 1)
    LASSERT_TYPE("tail", a, 0, LVAL_QEXPR)
    LASSERT_NOT_EMPTY("tail", a, 0)

    lval *v = lval_take(a, 0);
    lval_del(lval_pop(v, 0));
    return v;
}

lval *builtin_list(lenv *e, lval *a) {
    a->type = LVAL_QEXPR;
    return a;
}

lval *builtin_eval(lenv *e, lval *a) {
    LASSERT_NUM("eval", a, 1)
    LASSERT_TYPE("eval", a, 0, LVAL_QEXPR)

    lval *x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(e, x);
}

lval *builtin_join(lenv *e, lval *a) {
    for (int i = 0; i < a->count; i++) {
        LASSERT_TYPE("join", a, i, LVAL_QEXPR)
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
        LASSERT_TYPE(op, a, i, LVAL_NUM)
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
    LASSERT_TYPE(func, a, 0, LVAL_QEXPR)

    lval *syms = a->cell[0];
    for (int i = 0; i < syms->count; i++) {
        LASSERT(a, syms->cell[i]->type == LVAL_SYM,
                "Function 'def' cannot define non-symbol. Got %s, expected %s.",
                ltype_name(syms->cell[i]->type), ltype_name(LVAL_SYM))
    }

    LASSERT(a, syms->count == a->count - 1,
            "Function '%s' cannot take incorrect number of values to symbols. Got %s, expected %s",
            func, syms->count, a->count - 1)

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

lval *builtin_ord(lenv *e, lval *a, char *op) {
    // check that there are two arguments and they're both numbers
    LASSERT_NUM(op, a, 2)
    LASSERT_TYPE(op, a, 0, LVAL_NUM)
    LASSERT_TYPE(op, a, 1, LVAL_NUM)

    int r;
    if (strcmp(op, ">") == 0) {
        r = a->cell[0]->num > a->cell[1]->num;
    } else if (strcmp(op, "<") == 0) {
        r = a->cell[0]->num < a->cell[1]->num;
    } else if (strcmp(op, ">=") == 0) {
        r = a->cell[0]->num >= a->cell[1]->num;
    } else if (strcmp(op, "<=") == 0) {
        r = a->cell[0]->num <= a->cell[1]->num;
    } else {
        // should never happen
        return lval_err("'%s' is not a known ordering", op);
    }

    lval_del(a);
    return lval_num(r);
}

lval *builtin_gt(lenv *e, lval *a) {
    return builtin_ord(e, a, ">");
}

lval *builtin_lt(lenv *e, lval *a) {
    return builtin_ord(e, a, "<");
}

lval *builtin_ge(lenv *e, lval *a) {
    return builtin_ord(e, a, ">=");
}

lval *builtin_le(lenv *e, lval *a) {
    return builtin_ord(e, a, "<=");
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

