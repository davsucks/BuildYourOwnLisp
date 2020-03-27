#include "builtins.h"
#include "mpc.h"
#include "lval.h"
#include "lenv.h"
#include "main.h"
#include "parsing.h"

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

lval *builtin_cmp(lenv *e, lval *a, char *op) {
    LASSERT_NUM(op, a, 2)
    int r;
    if (strcmp(op, "==") == 0) {
        r = lval_eq(a->cell[0], a->cell[1]);
    } else if (strcmp(op, "!=") == 0) {
        r = !lval_eq(a->cell[0], a->cell[1]);
    } else {
        // shouldn't happen
        return lval_err("'%s' is unknown comparison.", op);
    }
    lval_del(a);
    return lval_num(r);
}

lval *builtin_eq(lenv *e, lval *a) {
    return builtin_cmp(e, a, "==");
}

lval *builtin_ne(lenv *e, lval *a) {
    return builtin_cmp(e, a, "!=");
}

lval *builtin_if(lenv *e, lval *a) {
    LASSERT_NUM("if", a, 3)
    LASSERT_TYPE("if", a, 0, LVAL_NUM)
    LASSERT_TYPE("if", a, 1, LVAL_QEXPR)
    LASSERT_TYPE("if", a, 2, LVAL_QEXPR)

    // mark both expressions as evaluable
    a->cell[1]->type = LVAL_SEXPR;
    a->cell[2]->type = LVAL_SEXPR;

    lval *result;
    if (a->cell[0]->num) {
        result = lval_eval(e, lval_pop(a, 1));
    } else {
        result = lval_eval(e, lval_pop(a, 2));
    }
    lval_del(a);
    return result;
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

lval *builtin_load(lenv *e, lval *a) {
    LASSERT_NUM("load", a, 1)
    LASSERT_TYPE("load", a, 0, LVAL_STR)

    // parse file given by string name
    mpc_result_t r;
    if (mpc_parse_contents(a->cell[0]->str, Lispy, &r)) {
        // read contents
        lval *expr = lval_read(r.output);
        mpc_ast_delete(r.output);

        // evaluate each expression
        while (expr->count) {
            lval *x = lval_eval(e, lval_pop(expr, 0));
            // if evaluation leads to an error print it
            if (x->type == LVAL_ERR) { lval_println(x); }
            lval_del(x);
        }

        // delete expressions and arguments
        lval_del(expr);
        lval_del(a);

        // return empty list
        return lval_sexpr();
    } else {
        // get parse error as string
        char *error_message = mpc_err_string(r.error);
        mpc_err_delete(r.error);

        // create new error message using it
        lval *err = lval_err("Could not load Library %s", error_message);
        free(error_message);
        lval_del(a);
        return err;
    }
}

lval *builtin_print(lenv *e, lval *a) {
    // print each argument followed by a space
    for (int i = 0; i < a->count; i++) {
        lval_print(a->cell[i]);
        putchar(' ');
    }

    // print newline and delete arguments
    putchar('\n');
    lval_del(a);

    return lval_sexpr();
}

lval *builtin_error(lenv *e, lval *a) {
    LASSERT_NUM("error", a, 1)
    LASSERT_TYPE("error", a, 0, LVAL_STR)

    // construct error from first argument
    lval *err = lval_err(a->cell[0]->str);

    // delete arguments and return
    lval_del(a);
    return err;
}
