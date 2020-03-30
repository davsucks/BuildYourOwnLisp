grammar Kotlispy;

/* Parser rules */

lispy: operator expression+ EOF ;

operator: PLUS | MINUS | MULT | DIV;

number: DIGIT+;

expression: number | '(' operator expression+ ')';

/* Lexer Rules */

PLUS: '+';
MINUS: '-' ;
MULT: '*';
DIV: '/';

DIGIT: [0-9];

WS: [\t ]+ -> skip;