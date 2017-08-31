%{
#include "parser.tab.h"
%}

%%
"declblock"		return declaration_starter;
"codeblock"		return code_starter;

\"[ -~]*\"	return STR_LIT;
[0-9][0-9]*		return NUM; 

"int"		return INT;
"while"		return WHILE;
"for"		return FOR;
"if"		return IF;
"else"		return ELSE;
"goto"		return GOTO;
"print"		return PRINT;
"println"	return PRINTLN;
"read"		return READ;

[a-zA-Z][a-zA-Z0-9]*   return ID;

"+"|"-"|"*"|"/"		return A_OP;
"=="|"!="|"<"|">"|"<="|">="    return C_OP;
"&&"|"||"    return L_OP;
"("		return '(';
")"		return ')';
"{"		return '{';
"}"		return '}';
"["		return '[';
"]"		return ']';
"="		return '=';
","		return ',';
":"		return ':';
";"     return ';';

[ \t\n]		{ /* Do nothing */ }
.		{ 
		  printf("Unexpected token encountered: %s\n", yytext); 
		  return ETOK;
		}
