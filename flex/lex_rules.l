
DIGIT      [0-9]
DIGITS     {DIGIT}+
LETTER     [a-zA-Z]

ID         {LETTER}({LETTER}|{DIGIT})*

NUM        {DIGITS}|{DIGITS}\.{DIGITS}(E{DIGITS}){0,1}

RELOP      \=\=|!\=|>|>\=|<|<\=

ASSIGN     \=

PRIMITIVE  boolean|int|float

FLOW   if|else|while

PUNCT  \;|\,|\(|\)|\[|\]|\{|\}

ADDOP  \+|\-

MULOP  \*|\\

%%

{PRIMITIVE}   printf( "%s\n", yytext );

{FLOW}        printf( "%s\n", yytext );

{ID}          printf( "id\n");
{NUM}         printf( "num\n" );
{RELOP}       printf( "relop\n");

{ASSIGN}      printf( "%s\n", yytext );
{PUNCT}       printf( "%s\n", yytext );
{ADDOP}       printf( "addop\n" );
{MULOP}       printf( "mulop\n");

.	       /*do nothing*/

%%

int main( int argc, char **argv )
{
    ++argv, --argc;	/* skip over program name */
    if ( argc > 0 )
        yyin = fopen( argv[0], "r" );
    else
        yyin = stdin;

    yylex();
}
