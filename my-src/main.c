#include "mpc.h"
#include "lenv.h"
#include "lval.h"
#include "builtins.h"
#include "parsing.h"

#ifdef _WIN32
#include <string.h>

static char buffer[2048];

/* Fake readline function */
char* readline(char* prompt) {
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char* cpy = malloc(strlen(buffer) + 1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy) - 1] = '\0';
    return cpy;
}

/* Fake add history function */
void add_history(char* unused) {}

#else

#include <editline/readline.h>

#endif

int main(int argc, char **argv) {

    /* Create some parsers */
    mpc_parser_t *Number = mpc_new("number");
    mpc_parser_t *Symbol = mpc_new("symbol");
    mpc_parser_t *String = mpc_new("string");
    mpc_parser_t *Comment = mpc_new("comment");
    mpc_parser_t *Sexpr = mpc_new("sexpr");
    mpc_parser_t *Qexpr = mpc_new("qexpr");
    mpc_parser_t *Expr = mpc_new("expr");
    mpc_parser_t *Lispy = mpc_new("lispy");

    /* Define them with the following Language */
    mpca_lang(MPCA_LANG_DEFAULT,
              "                                         \
                number: /-?[0-9]+/ ;                            \
                symbol: /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;      \
                string: /\"(\\\\.|[^\"])*\"/ ;                  \
                comment : /;[^\\r\\n]*/ ;                       \
                sexpr: '(' <expr>* ')' ;                        \
                qexpr: '{' <expr>* '}' ;                        \
                expr: <number> | <symbol> | <string> | <comment> | <sexpr> | <qexpr> ; \
                lispy: /^/ <expr>* /$/ ;                        \
            ",
              Number, Symbol, String, Comment, Sexpr, Qexpr, Expr, Lispy);

    /* Print Version and Exit information */
    puts("Lispy Version 0.0.0.0.1");
    puts("Press Ctrl+c or type 'exit' to Exit\n");

    lenv *e = lenv_new();
    lenv_add_builtins(e);

    /* In a never ending loop */
    while (1) {

        /* Output our prompt and get input */
        char *input = readline("lispy> ");

        if (strcmp(input, "exit") == 0) {
            break;
        }

        /* Add input to history */
        add_history(input);

        /* Attempt to parse the user input */
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            lval *x = lval_eval(e, lval_read(r.output));
            lval_println(x);
            lval_del(x);
        } else {
            /* Otherwise print the error */
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        /* Free retrieved input */
        free(input);
    }


    /* Undefine and Delete our Parsers and environment */
    mpc_cleanup(8, Number, Symbol, String, Comment, Sexpr, Qexpr, Expr, Lispy);
    lenv_del(e);

    return 0;
}

