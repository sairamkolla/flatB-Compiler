%{
#include "head.h"
#include "ast.h"
extern FILE *yyin;
void yyerror(string s);
int yylex(void);
using namespace std;
ASTProgramNode* root;
extern Module *flatBToLLVM;
extern IRBuilder<> *Builder;
%}

%union
{
    int     intVal;
    string  *strVal;

    ASTProgramNode*						prog;
    ASTExpressionNode*                  expr;       
    ASTLocationNode*                    loc;    
    ASTIntegerLiteralExpressionNode*    intLit;                     
    ASTBoolLiteralExpressionNode*       boolLit;                    
    ASTBinaryExpressionNode*            binexpr;                
    ASTUnaryExpressionNode*             unexpr;         
    ASTStatementDeclNode*               stmt;           
    ASTBlock*                           block;
    ASTStatementDeclListNode*	        stmt_list;
    ASTPrintLitNode*                    printlit;
    ASTPrintNode*						printlit_list;
    ASTLabelDeclNode*					label_decl;		
    Symbol* 							sym;
    list<Symbol*>* 						sym_list;

    /*
    ASTParameterDecl* 					param_decl;

    ASTParameterDecl* 					param_decl;
	ASTParameterDeclListNode* 			param_decl_list;
	ASTDeclBlockNode* 					declblock;
	*/

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
%type 	<strVal>	CODE_STARTER
%type 	<strVal> 	TRUE
%type 	<strVal> 	FALSE
%type 	<strVal>	DECLARATION_STARTER
%type 	<strVal> 	PRINT
%type 	<strVal> 	PRINTLN
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

%type <prog>			Program;
%type <expr>        	Expr;
%type <loc>         	Location;
%type <intLit>      	IntegerLiteral;
%type <boolLit>     	BoolLiteral;
%type <binexpr>     	BinExpr;
%type <stmt>        	StatementDecl;
%type <block>       	CodeBlock;  
%type <stmt_list>   	StatementDecl_List;
%type <printlit>   		printLit;
%type <printlit_list>  	Print_Statement;
%type <printlit_list>  	printLit_list;
%type <label_decl>		Label;
%type <sym>				VariableDecl;
%type <intVal> 			Type;

%type <sym_list> 		VariableDecl_List;







%%

Program				: DECLARATION_STARTER DeclBlock CODE_STARTER CodeBlock {$$ = new ASTProgramNode($4); root = $$;}
					;

DeclBlock			: '{' DeclList '}' 
					| '{' '}' 
					;
/* Grammer for Declaration block, only constant expr are allowed herer */
DeclList			: DeclList DeclLine 
					| DeclLine 
					;

DeclLine			: Type VariableDecl_List ';' {annotateSymbolTable($1,$2);}
					;

Type 				: DATA_TYPE { if(!($1->compare("int"))) $$ = _int_; else $$ = _bool_;} 

VariableDecl_List	: VariableDecl {$$ = new list<Symbol*>(); $$->push_back($1);}
					| VariableDecl_List ',' VariableDecl {$$ = $1; $$->push_back($3);}
					;

VariableDecl 		: ID { $$ = new Symbol(*($1));}
					| ID '[' IntegerLiteral ']' {$$ = new Symbol(*($1),$3);} 
					;
/* Grammer for CodeBlock */

// Basic Definitions
Location 	 		: ID { $$ = new ASTLocationNode(*($1));}
					| ID '[' Expr ']'{ $$ = new ASTLocationNode(*($1),$3);} 
					;

IntegerLiteral 		: DEC_LITERAL { $$ = new ASTIntegerLiteralExpressionNode($1) ; }
					| HEX_LITERAL { $$ = new ASTIntegerLiteralExpressionNode($1) ; }
					;

BoolLiteral			: TRUE { $$ = new ASTBoolLiteralExpressionNode(*($1));}
					| FALSE{ $$ = new ASTBoolLiteralExpressionNode(*($1));}
					;


CodeBlock			: '{' StatementDecl_List '}' {$$ = new ASTBlock($2);}
					| '{' '}'	{$$ = new ASTBlock();}
					;

StatementDecl_List	: StatementDecl { $$ = new ASTStatementDeclListNode();$$->push($1);}
					| StatementDecl_List StatementDecl {$$ = $1; $$->push($2);}
					;


StatementDecl		: Print_Statement ';' {$$ = $1;}
					| Location ASSIGN Expr ';' {$$ = new ASTAssignmentStatementNode($1,$3,*($2));}
					//| FOR ID ASSIGN ConstExpr ',' ConstExpr CodeBlock
					//| FOR ID ASSIGN ConstExpr ','  ConstExpr ',' ConstExpr CodeBlock
					| FOR ID ASSIGN Expr ',' Expr CodeBlock {$$ = new ASTForStatementDeclNode(*($2),$4,$6,$7);}
					| FOR ID ASSIGN Expr ','  Expr ',' Expr CodeBlock {$$ = new ASTForStatementDeclNode(*($2),$4,$6,$8,$9);}
					| IF Expr CodeBlock {$$ = new ASTIfStatementDeclNode($2,$3);}
					| IF Expr CodeBlock ELSE  CodeBlock {$$ = new ASTIfStatementDeclNode($2,$3,$5);}
					| WHILE Expr CodeBlock {$$ = new ASTWhileStatementDeclNode($2,$3);}
					| Label ':' 
					| GOTO Label ';' { $$ = new ASTGotoDeclNode($2->get_label_name());}
					| GOTO Label IF Expr ';' {$$ = new ASTGotoDeclNode($2->get_label_name(),$4);}
					| READ Location ';'{$$ = new ASTReadNode($2);}
					;

Label				: ID { 	}
					;

Expr 				: Location {$$=new ASTLocationExpressionNode($1);}
					| IntegerLiteral {$$ = $1;}
					| MINUS Expr %prec UNARY
					| NOT Expr
					| BoolLiteral {$$ = $1;}
					| '(' Expr ')' { $$ = $2;}
					| BinExpr	{ $$ = $1;}			
					;
/** DIFFERENT TYPE OF EXPRESSIONS **/
/* Bin expr is grammer which denotes Non boolean expressions */
BinExpr				: Expr MULT Expr	{$$ = new ASTBinaryExpressionNode($1,*($2),$3);}
					| Expr DIV Expr		{$$ = new ASTBinaryExpressionNode($1,*($2),$3);}
					| Expr MOD Expr		{$$ = new ASTBinaryExpressionNode($1,*($2),$3);}
					| Expr PLUS Expr	{$$ = new ASTBinaryExpressionNode($1,*($2),$3);}
					| Expr MINUS Expr	{$$ = new ASTBinaryExpressionNode($1,*($2),$3);}
					| Expr AND Expr		{$$ = new ASTBinaryExpressionNode($1,*($2),$3);}
					| Expr OR Expr		{$$ = new ASTBinaryExpressionNode($1,*($2),$3);}
					| Expr GT Expr		{$$ = new ASTBinaryExpressionNode($1,*($2),$3);}
					| Expr GTEQ Expr	{$$ = new ASTBinaryExpressionNode($1,*($2),$3);}
					| Expr EQ Expr		{$$ = new ASTBinaryExpressionNode($1,*($2),$3);}
					| Expr NEQ Expr		{$$ = new ASTBinaryExpressionNode($1,*($2),$3);}
					| Expr LT Expr		{$$ = new ASTBinaryExpressionNode($1,*($2),$3);}
					| Expr LTEQ Expr	{$$ = new ASTBinaryExpressionNode($1,*($2),$3);}
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

Print_Statement		: PRINTLN printLit_list {$$ = $2; $$->setType(*($1));}
					| PRINT printLit_list {$$ = $2; $$->setType(*($1));}
					;


printLit_list		: printLit {$$ = new ASTPrintNode(); $$->push($1);}
					| printLit ',' printLit_list {$$ = $3;$$->push($1);}
					;

printLit			: Expr { $$ = new ASTPrintLitNode($1);}
					| STR_LITERAL { $$ = new ASTPrintLitNode(*($1));}
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
	//ASTNode* temp = new ASTNode();
	//temp->test();
	Builder = new IRBuilder<>(getGlobalContext());
    flatBToLLVM = new Module("flatBToLLVM", getGlobalContext());
	yyparse();
	cout << "Lexical analysis successfull" << endl;
	Interpreter evaluator;
    root->accept(&evaluator);
    flatBToLLVM->dump();
    return 0;
}
