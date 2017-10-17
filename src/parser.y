%{
#include "head.h"
#include "ast.h"
extern FILE *yyin;
void yyerror(string s);
int yylex(void);
using namespace std;
int var_type[9999];
unordered_map<string,int> var_index;
unordered_map<string,int> vardict;
unordered_map<string,int*> arrdict;
bool CheckAvail(string s);
bool CheckAvail(string s);
void update_var_value(string s,int value);
void update_arr_value(string s, int index,int value);
int getType(string s);
void insert_var(string s);
void insert_arr(string s,int size);
void insert_label(string s);
ASTBlock* root;
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
    //ASTParameterDecl*                 param;
    //list<ASTParameterDecl *>          param_list;
    ASTStatementDeclListNode*	        stmt_list;
    Symbol*                             sym;
    ASTPrintLitNode*                    printlit;
    ASTPrintNode*						printlit_list;
    ASTLabelDeclNode*					label_decl;					
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

%type <block>			Program;
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
%type <sym>         	VariableDecl;
%type <label_decl>		Label;



%%

Program				: DECLARATION_STARTER DeclBlock CODE_STARTER CodeBlock {$$ = $4;root = $$; cout << "declared the program" << endl;}
					;

DeclBlock			: '{' DeclList '}' {}
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

VariableDecl 		: ID {
						bool x = CheckAvail(*($1));
						if(x){
							insert_var(*($1));
						}
						else{
							cout << "Redeclaration of " << *($1)  << endl;
						}
					}
					| ID '[' IntegerLiteral ']' {
						bool x = CheckAvail(*($1));
						if(x){
							insert_arr(*($1),$3->getValue());
						}
						else{
							cout << "Redeclaration of " << *($1)  << endl;
						}	
					} 
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


CodeBlock			: '{' StatementDecl_List '}' {$$ = new ASTBlock($2);cout << "declared the codeblock" << endl;}
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

Label				: ID { 
						bool x = CheckAvail(*($1));
						if(x){
							vardict[*($1)] = 0;
							$$ = new class ASTLabelDeclNode(*($1));
						}else{
							cout << " Redeclaration of label " << *($1) << endl;
						}
					}
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

bool CheckAvail(string s){
	/*
	unordered_map<string,int> var_index;
	unordered_map<string,int> vardict;
	unordered_map<string,int*> arrdict;
	int var_type[9999];
	*/
	unordered_map<string,int>::const_iterator got = var_index.find (s);
	if(var_index.size()==0)
		return true;
	if(got == var_index.end())
		return true;
	return false;
}

void update_var_value(string s,int value){
	vardict[s] = value;
}

void update_arr_value(string s, int index,int value){
	//unordered_map<string,int>::const_iterator got = var_index.find (s);
	//(got->second)[index] = value;
	arrdict[s][index] = value;
}

int getType(string s){
	unordered_map<string,int>::const_iterator got = var_index.find (s);
	return var_type[got->second];
}

void insert_var(string s){
	/*
	1 - VARIABLE
	2 - ARRAY
	3 - LABEL
	*/
	int temp1 = vardict.size();
	temp1 += arrdict.size();	
	var_type[temp1] = 1;
	var_index[s] = temp1;
	vardict[s] = 0;
	cout << "Added variable " << s << endl;
}

void insert_arr(string s,int size){
	int temp1 = vardict.size();
	temp1 += arrdict.size();	
	var_type[temp1] = 2;
	var_index[s] = temp1;
	arrdict[s] = new int[size];
	cout << "Added array " << s << " of size " << size << endl;

}

void insert_label(string s){
	int temp1 = vardict.size();
	temp1 += arrdict.size();	
	var_type[temp1] = 3;
	var_index[s] = temp1;
	vardict[s] = 0;
}



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
	yyparse();
	Interpreter* interpreter = new Interpreter();
    root->accept(interpreter);
}
