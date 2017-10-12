%{
#include "head.h"
#include "ast.h"
#include "parser.tab.h"
void yyerror(string s);
%}


%option yylineno
%option noyywrap


SINGLE_QUOTES 			"\'"
DOUBLE_QUOTES 			"\""
COLON 					":"
SEMICOLON 				";"
COMMA 					","
OPENROUND 				"\("
CLOSEROUND 				"\)"
OPENSQUARE 				"\["
CLOSESQUARE 			"\]"
OPENCURLY 				"\{"
CLOSECURLY 				"\}"


WHITESPACES 			[ \t\n]+


/*Regex related to special characters and operators*/

NOT 					"!"
PLUS 					"+"
MINUS 					"-"
UNARY 					'-'
MULT 					"\*"
DIV 					"/"
MOD 					"%"
AND 					"&&"
OR 						"||"
ASSIGN 					"="
EQ 						"=="
NEQ 					"!="
GT 						">"
LT 						"<"
GTEQ 					">="
LTEQ 					"<="

/* Regular Expressions related to handling numbers */

DEC_LITERAL 			[0-9]+
HEX_LITERAL 			0[xX][0-9a-fA-F]+

/* Regular Expressions related to handling names */
DECLARATION_STARTER		"declblock"		
CODE_STARTER			"codeblock"		
IF                      "if"
ELSE                    "else"
BREAK                   "break"
CONTINUE                "continue"
FOR                     "for"
WHILE					"while"
ID 						[a-zA-Z][a-zA-Z0-9]*
PRINT 					"print"
PRINTLN					"println"
GOTO 					"goto"
READ 					"read"

/* Regular Expressions related to handling types */
INTEGER 				"int"
BOOLEAN 				"bool"
TRUE 					"true"
FALSE 					"false"

%%

\"[ -~]*\"				{ yylval.strVal = new string(yytext);return STR_LITERAL;}
{DECLARATION_STARTER}	{ return DECLARATION_STARTER;}
{CODE_STARTER}			{ return CODE_STARTER;}

{INTEGER}				{ yylval.strVal = new string(yytext); return DATA_TYPE;}
{BOOLEAN}				{ yylval.strVal = new string(yytext); return DATA_TYPE;}
{TRUE}					{ yylval.strVal = new string(yytext); return TRUE;}
{FALSE}					{ yylval.strVal = new string(yytext); return FALSE;}

{PLUS}					{ yylval.strVal = new string(yytext); return PLUS;}
{MINUS}					{ yylval.strVal = new string(yytext); return MINUS;}
{UNARY}					{ yylval.strVal = new string(yytext); return UNARY;}
{MULT}					{ yylval.strVal = new string(yytext); return MULT;}
{DIV}					{ yylval.strVal = new string(yytext); return DIV;}
{MOD}					{ yylval.strVal = new string(yytext); return MOD;}

{AND}					{ yylval.strVal = new string(yytext); return AND;}
{OR}					{ yylval.strVal = new string(yytext); return OR;}

{ASSIGN}				{ yylval.strVal = new string(yytext); return ASSIGN;}
{EQ}					{ yylval.strVal = new string(yytext); return EQ;}
{NEQ}					{ yylval.strVal = new string(yytext); return NEQ;}
{GT}					{ yylval.strVal = new string(yytext); return GT;}
{LT}					{ yylval.strVal = new string(yytext); return LT;}
{GTEQ}					{ yylval.strVal = new string(yytext); return GTEQ;}
{LTEQ}					{ yylval.strVal = new string(yytext); return LTEQ;}
{NOT}					{ yylval.strVal = new string(yytext); return NOT;}

{READ}					{ yylval.strVal = new string(yytext); return READ;}
{PRINTLN}				{ yylval.strVal = new string(yytext); return PRINTLN;}
{PRINT}					{ yylval.strVal = new string(yytext); return PRINT;}


{DEC_LITERAL}			{ yylval.intVal = atoi(yytext); return DEC_LITERAL;}
{HEX_LITERAL}			{ sscanf(yytext, "%d", &yylval.intVal); return HEX_LITERAL;}



{IF}					{ return IF;}
{ELSE}					{ return ELSE;}
{FOR}					{ return FOR;}
{WHILE}					{ return WHILE;}
{GOTO}					{ return GOTO;}
{BREAK}					{ return BREAK;}
{CONTINUE}				{ return CONTINUE;}
{ID}					{ yylval.strVal = new string(yytext); return ID;}

[ \t\n]						{ }
.						{ return yytext[0];}

%%