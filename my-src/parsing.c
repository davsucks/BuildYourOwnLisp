#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"
#include "lval.h"

lval *lval_read_num(mpc_ast_t *t) {
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ?
           lval_num(x) : lval_err("invalid number");
}

lval *lval_read_str(mpc_ast_t *t) {
    // cut off the final quote character
    t->contents[strlen(t->contents) - 1] = '\0';
    // copy the string skipping the first quote character
    char *unescaped = malloc(strlen(t->contents + 1) + 1);
    strcpy(unescaped, t->contents + 1);
    // pass through the unescape function
    unescaped = mpcf_unescape(unescaped);
    // construct new lval using the string
    lval *str = lval_str(unescaped);
    // cleanup and return
    free(unescaped);
    return str;
}

lval *lval_read(mpc_ast_t *t) {
    /* If symbol or number return conversion to that type */
    if (strstr(t->tag, "number")) { return lval_read_num(t); }
    if (strstr(t->tag, "symbol")) { return lval_sym(t->contents); }
    if (strstr(t->tag, "string")) { return lval_read_str(t); }

    /* If root (>) or sexpr then create empty list */
    lval *x = NULL;
    if (strcmp(t->tag, ">") == 0) { x = lval_sexpr(); }
    if (strstr(t->tag, "sexpr")) { x = lval_sexpr(); }
    if (strstr(t->tag, "qexpr")) { x = lval_qexpr(); }

    /* Fill this list with any valid expressions contained within */
    for (int i = 0; i < t->children_num; i++) {
        if (strstr(t->children[i]->tag, "comment")) { continue; }
        if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
        if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
        if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
        if (strcmp(t->children[i]->contents, "}") == 0) { continue; }
        if (strcmp(t->children[i]->tag, "regex") == 0) { continue; }
        x = lval_add(x, lval_read(t->children[i]));
    }
    return x;
}
