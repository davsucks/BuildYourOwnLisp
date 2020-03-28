grammar Kotlispy;

/* Parser rules */

lispy: (OPERATOR expression)+ ;

expression: NUMBER | '(' lispy ')';

/* Lexer Rules */

OPERATOR: '+' | '-' | '*' | '/';

NUMBER: [0-9]+;
