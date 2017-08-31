%{
  #include <stdio.h>
  #include <stdlib.h>
  FILE *yyin;
  int yylex (void);
  void yyerror (char const *s);
%}

%token declaration_starter
%token code_starter
%token NUM
%token STR_LIT
%token INT
%token WHILE
%token FOR
%token IF
%token ELSE
%token GOTO
%token PRINT
%token PRINTLN
%token READ
%token ID
%token ETOK
%left A_OP
%left C_OP
%left L_OP

%%

program	: declaration_starter decl_block code_starter CodeBlock
	;

decl_block	: '{' decl_list '}'
			| '{' '}'
			;

decl_list	: decl_list decl_line
			| decl_line
         	;

decl_line	: D_TYPE VAR_LIST ';'
			;

D_TYPE	: INT
		;

VAR_LIST	: VAR_LIST ',' VAR
			| VAR
			;

VAR		: ID '[' Expr ']'
		| ID
		;

CodeBlock	: '{' StatementList '}'
			| '{' '}'
			;
StatementList	:StatementList Statement
				| Statement
				
				;

Statement	: VAR '=' Expr ';'
			| Expr ';'
			| IO_Statement ';'
			| CodeBlock
			| FOR ID '=' NUM ',' NUM CodeBlock
			| FOR ID '=' NUM ',' NUM ',' NUM CodeBlock
			| IF BoolExprs CodeBlock
			| IF BoolExprs CodeBlock ELSE CodeBlock
			| WHILE BoolExprs CodeBlock
			| Label ':' Statement
			| GOTO Label ';'
			| GOTO Label  IF BoolExprs ';'	
			;

Label		: ID
		;

IO_Statement	: PRINTLN print_list
				| PRINT print_list
				| READ VAR
				;

print_list	: print_list ',' print_lit
			| print_lit
			;

print_lit	: Expr
			| STR_LIT
			;

BoolExprs	: BoolExprs L_OP BoolExpr
			| BoolExpr
			;

BoolExpr	: Expr C_OP Expr
			| '(' BoolExprs ')'
		;

Expr	: Expr A_OP Expr
		| '(' Expr ')'
		| VAR
		| NUM
		;


%%

void yyerror (char const *s)
{
       fprintf (stderr, "YIKES!, parse error!: %s\n", s);
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

	yyparse();
}
