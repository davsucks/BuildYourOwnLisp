grammar Kotlispy;

/* Parser rules */

lispy: OPERATOR expression+ EOF ;

expression: NUMBER | '(' OPERATOR expression+ ')';

/* Lexer Rules */

OPERATOR: '+' | '-' | '*' | '/';

NUMBER: [0-9]+;

WS: [\t ]+ -> skip;