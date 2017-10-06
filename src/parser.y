%{
#include "head.h"
extern FILE *yyin;
void yyerror(string s);
int yylex(void);
%}

%union{
    int     intVal;
    string  *strVal;


}


%token DECLARATION_STARTER CODE_STARTER 
%token WHILE FOR
%token IF ELSE BREAK CONTINUE
%token GOTO PRINT PRINTLN READ
%token TRUE FALSE ID
%token ETOK
%token DATA_TYPE 
%token ASSIGN EQ NEQ GT LT GTEQ LTEQ
%token DEC_LITERAL HEX_LITERAL	 STR_LITERAL


%left PLUS MINUS UNARY
%left MULT DIV MOD
%left AND OR
%left NOT
%left EQ NEQ GT LT GTEQ LTEQ



%type 	<strVal>	ID
%type 	<strVal>	DATA_TYPE
%type 	<intVal>	DEC_LITERAL
%type 	<intVal>	HEX_LITERAL
%type 	<strVal>	STR_LITERAL
%type 	<strVal>	ASSIGN
%type 	<strVal>	MINUS
%type 	<strVal>	NOT
%type 	<strVal>	MULT
%type 	<strVal>	DIV
%type 	<strVal>	PLUS
%type 	<strVal>	MOD
%type 	<strVal>	AND
%type 	<strVal>	OR
%type 	<strVal>	EQ
%type 	<strVal>	NEQ
%type 	<strVal>	GT
%type 	<strVal>	LT
%type 	<strVal>	GTEQ
%type 	<strVal>	LTEQ



%%

Program				: DECLARATION_STARTER DeclBlock CODE_STARTER CodeBlock
					;

DeclBlock			: '{' DeclList '}' 
					| '{' '}' 
					;
/* Grammer for Declaration block, only constant expr are allowed herer */
DeclList			: DeclList DeclLine
					| DeclLine
					;

DeclLine			: DATA_TYPE VariableDecl_List ';' 
					;

VariableDecl_List	: VariableDecl 
					| VariableDecl_List ',' VariableDecl
					;

VariableDecl 		: ID
					| ID '[' IntegerLiteral ']' 
					;
/* Grammer for CodeBlock */

// Basic Definitions
Location 	 		: ID
					| ID '[' Expr ']' 
					;

IntegerLiteral 		: DEC_LITERAL
					| HEX_LITERAL
					;

BoolLiteral			: TRUE
					| FALSE
					;


CodeBlock			: '{' StatementList '}' 
					| '{' '}'
					;

StatementList		: StatementDecl 
					| StatementList StatementDecl
					;


StatementDecl		: IO_Statement ';'
					| Location ASSIGN Expr ';'
					//| FOR ID ASSIGN ConstExpr ',' ConstExpr CodeBlock
					//| FOR ID ASSIGN ConstExpr ','  ConstExpr ',' ConstExpr CodeBlock
					| FOR ID ASSIGN IntegerLiteral ',' IntegerLiteral CodeBlock
					| FOR ID ASSIGN IntegerLiteral ','  IntegerLiteral ',' IntegerLiteral CodeBlock
					| IF Expr CodeBlock
					| IF Expr CodeBlock ELSE  CodeBlock
					| WHILE Expr CodeBlock
					| Label ':' StatementDecl
					| GOTO Label ';'
					| GOTO Label IF Expr ';'
					;

Label				: ID
					;

Expr 				: Location
					| IntegerLiteral
					| MINUS Expr %prec UNARY
					| NOT Expr
					| BoolLiteral
					| '(' Expr ')'
					| BinExpr					
					;
/** DIFFERENT TYPE OF EXPRESSIONS **/
/* Bin expr is grammer which denotes Non boolean expressions */
BinExpr				: Expr MULT Expr
					| Expr DIV Expr
					| Expr MOD Expr
					| Expr PLUS Expr
					| Expr MINUS Expr
					| Expr AND Expr
					| Expr OR Expr
					| Expr GT Expr
					| Expr GTEQ Expr
					| Expr EQ Expr
					| Expr NEQ Expr
					| Expr LT Expr
					| Expr LTEQ Expr
					;

/* Const Expr is grammer for expressions which don't involve variables, Used during declarations */

/*
ConstExpr			: '(' ConstExpr ')'
					| DEC_LITERAL
					| ConstExpr MULT ConstExpr
					| ConstExpr DIV ConstExpr
					| ConstExpr MOD ConstExpr
					| ConstExpr PLUS ConstExpr
					| ConstExpr MINUS ConstExpr
					;
*/

IO_Statement		: PRINTLN print_list 
					| PRINT print_list 
					| READ Location 
					;


print_list			: print_lit
					| print_lit ',' print_list
					;

print_lit			: Expr
					| STR_LITERAL
					;




%%


void yyerror (string s){
	extern int yylineno;	// defined and maintained in lex.c
    extern char *yytext;	// defined and maintained in lex.c

    cout << "ERROR: " << s << " at symbol \"" << yytext;
    cout << "\" on line " << yylineno << endl;
    exit(1);
}
int main(int argc, char *argv[]){
	if(argc == 1){
		fprintf(stderr, "Correct usage: bcc filename\n");
		exit(1);
	}
	if(argc > 2){
		fprintf(stderr, "Passing more arguments than necessary.\n");
		fprintf(stderr, "Correct usage: bcc filename\n");
	}
	yyin = fopen(argv[1], "r");
	cout << "Parsing started for " << argv[1] << endl;
	yyparse();
}