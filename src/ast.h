#include <iostream>
#include <stdlib.h>
#include <string>
#include <map>
#include <list>
#include "stdllvm.h"

using namespace std;
using namespace llvm;


const int _int_ = 1;
const int _bool_ = 2;
const int _void_ = 3;

const int _error = 0;
const int _mult = 4;
const int _div = 8;
const int _plus = 16;
const int _minus = 32;
const int _eq = 64;
const int _neq = 128;
const int _and = 256;
const int _or = 512;
const int _gt = 1024;
const int _lt = 2048;
const int _gteq = 4096;
const int _lteq = 8192;
const int _unaryminus = 16384;
const int _negate = 32768;
const int _mod = 65536;

enum exprType { binary = 1, location = 2, literal = 3 , Unary = 4};
enum statementType { assign_statement = 1, for_statement = 2, if_statement = 3, while_statement = 4, 
	label_statement = 5, goto_statement = 6, print_statement = 8, read_statement = 9,default_statement=0};


class Symbol;

class ASTNode {
public:
	virtual Value *accept(class Visitor *) = 0;
};


class ASTExpressionNode : public ASTNode {
	public:
		ASTExpressionNode(exprType expr_type){
			expr_type_ = expr_type;
		}

		//virtual Value *accept(Visitor *)override;	
	private:
		exprType expr_type_;
};

class ASTLocationNode : public ASTNode {
	public:
		ASTLocationNode(string id)  {
			var_ = id;
			is_array_ = false;
		}
		ASTLocationNode(string id, ASTExpressionNode* exp){
			var_ = id;
			expr_ = exp;
			is_array_ = true;
		}
		string getVarName() const {
			return var_;
		}
		bool getType(){
			return is_array_;
		}
		ASTExpressionNode* getExpression(){
			return expr_;
		}
		Value *accept(Visitor *) override;
		
	private:
		string var_;
		bool is_array_;
		ASTExpressionNode* expr_;

};

class ASTIntegerLiteralExpressionNode : public ASTExpressionNode{
	public:
		ASTIntegerLiteralExpressionNode(int value): ASTExpressionNode(literal){
			value_ = value;
		}
		int getValue(){
			return value_;
		}
		Value *accept(Visitor *) override;
		
	private:
		int value_;
};

class ASTBoolLiteralExpressionNode : public ASTExpressionNode {
	public:
		ASTBoolLiteralExpressionNode(string val) : ASTExpressionNode(literal) {
			if(val == "true")
				value_ = true;
			else
				value_ = false;
		}
		bool getValue() const {
			return value_;
		}
		Value *accept(Visitor *) override;

	private:
		bool value_;
};


class ASTLocationExpressionNode : public ASTExpressionNode {
	public:
		ASTLocationExpressionNode(ASTLocationNode *loc) : ASTExpressionNode(location) {
			location_ = loc;
		}
		ASTLocationNode *getLocation() const {
			return location_;
		}
		Value *accept(Visitor *) override;

	private:
		ASTLocationNode *location_;
};



class ASTBinaryExpressionNode : public ASTExpressionNode{
	public:
		ASTBinaryExpressionNode(ASTExpressionNode* lhs, string op, ASTExpressionNode* rhs) : ASTExpressionNode(binary) {
			lhs_ = lhs;
			rhs_ = rhs;
			((!op.compare("*")) ? operator_ = _mult :
				((!op.compare("/")) ? operator_ = _div  :
				((!op.compare("+")) ? operator_ = _plus :
				((!op.compare("-")) ? operator_ = _minus :
				((!op.compare("%")) ? operator_ = _mod :
				((!op.compare("==")) ? operator_ = _eq :
				((!op.compare("!=")) ? operator_ = _neq :
				((!op.compare("&&")) ? operator_ = _and :
				((!op.compare("||")) ? operator_ = _or :
				((!op.compare(">")) ? operator_ = _gt :
				((!op.compare("<")) ? operator_ = _lt :
				((!op.compare(">=")) ? operator_ = _gteq :
				((!op.compare("<=")) ? operator_ = _lteq : operator_ = _error)))))))))))));
		}
		int getOperatorId(){
			return operator_;
		}
		ASTExpressionNode* getLHS(){
			return lhs_;
		}
		ASTExpressionNode* getRHS(){
			return rhs_;
		}
		Value *accept(Visitor *) override;
	private:
		ASTExpressionNode* lhs_;
		ASTExpressionNode* rhs_;
		int operator_;
};

class ASTUnaryExpressionNode : public ASTExpressionNode{
	public:
		ASTUnaryExpressionNode(string op,ASTExpressionNode* expr) : ASTExpressionNode(Unary){
			expr_ = expr;
			((!op.compare("-")) ? operator_ = _unaryminus :
			((!op.compare("!")) ? operator_ = _negate : operator_ = _error));
		}
		int getOperatorId(){
			return operator_;
		}

	private:
		ASTExpressionNode* expr_;
		int operator_;
};

class ASTStatementDeclNode : public ASTNode {
	public:
		ASTStatementDeclNode(statementType id) : statementId_(id) {}
		ASTStatementDeclNode() : statementId_(default_statement){}
		virtual statementType get_stmt_type(){
			return statementId_;
		}
		virtual string get_label_name(){
            return "None";
        };
		
	private:
		statementType statementId_;
};

class ASTStatementDeclListNode : public ASTNode{
	public:
		ASTStatementDeclListNode(){
			
		};

		void push(ASTStatementDeclNode* temp){
			//cout << "here finally" << endl;
			(stmts_).push_back(temp);
			//cout << "getting outta here!" << endl;
		}
		list<ASTStatementDeclNode *>* getStatementList() {
			return &stmts_;
		}
		Value *accept(Visitor *) override;
	private:
		list<ASTStatementDeclNode *> stmts_;
};



class ASTAssignmentStatementNode : public ASTStatementDeclNode{
	public:
		ASTAssignmentStatementNode(ASTLocationNode* loc,ASTExpressionNode* expr, string op) : ASTStatementDeclNode(assign_statement){
			location_ = loc;
			expr_ = expr;
			operator_ = op;

		}
		ASTLocationNode* getLocation(){
			return location_;
		}
		ASTExpressionNode* getExpression(){
			return expr_;
		}
		string getOperator(){
			return operator_;
		}
		Value *accept(Visitor *) override;
	private:
		ASTLocationNode* location_;
		ASTExpressionNode* expr_;
		string operator_;
};



class ASTBlock : public ASTNode {
	public:
		ASTBlock(ASTStatementDeclListNode *temp) {
			statementList_ = temp;
		};
		ASTBlock() {
			statementList_ = NULL;
		};
		ASTStatementDeclListNode *getStatementList() {
			return statementList_;
		}
		Value *accept(Visitor *) override;
	private:
		ASTStatementDeclListNode *statementList_;
		
};


class ASTProgramNode : public ASTNode {
	public:
		ASTProgramNode(ASTBlock* code_block){
			code_block_ = code_block;
		};
		ASTBlock* getCodeBlock(){
			return code_block_;
		}
		Value *accept(Visitor *) override;

	private:
		ASTBlock *code_block_;
};



class ASTIfStatementDeclNode : public ASTStatementDeclNode {
	public:
		ASTIfStatementDeclNode(ASTExpressionNode *ifExp, ASTBlock *ifBlock, ASTBlock *elseBlock) : ASTStatementDeclNode(if_statement) {
			ifExpression_ = ifExp;
			ifBlock_ = ifBlock;
			elseBlock_ = elseBlock;
			else_exists_ = true;
		}
		ASTIfStatementDeclNode(ASTExpressionNode *ifExp, ASTBlock *ifBlock) : ASTStatementDeclNode(if_statement) {
			ifExpression_ = ifExp;
			ifBlock_ = ifBlock;
			else_exists_ = false;
		}
		bool getType(){
			return else_exists_;
		}
		ASTExpressionNode *getExpression() const {
			return ifExpression_;
		}
		ASTBlock *getIfBlock() const {
			return ifBlock_;
		}
		ASTBlock *getElseBlock() const {
			return elseBlock_;
		}
		
		Value *accept(Visitor *) override;
	private:
		bool else_exists_;
		ASTExpressionNode *ifExpression_;
		ASTBlock *ifBlock_;
		ASTBlock *elseBlock_;
};

class ASTForStatementDeclNode : public ASTStatementDeclNode {
	public:
		ASTForStatementDeclNode(string it, ASTExpressionNode *start, ASTExpressionNode *step, ASTExpressionNode *end,ASTBlock *b) : ASTStatementDeclNode(for_statement) {
			iterName_ = it;
			initExpression_ = start;
			finalExpression_ = end;
			block_ = b;
			stepSize_ = step;
		}
		ASTForStatementDeclNode(string it, ASTExpressionNode *start, ASTExpressionNode *end,ASTBlock *b) : ASTStatementDeclNode(for_statement) {
			iterName_ = it;
			initExpression_ = start;
			finalExpression_ = end;
			block_ = b;
			stepSize_ = new ASTIntegerLiteralExpressionNode(1);
		}
		
		const string getIterVarName() const {
			return iterName_;
		}
		ASTExpressionNode *getInitExpression() const {
			return initExpression_;
		}
		ASTExpressionNode *getFinalExpression() const {
			return finalExpression_;
		}
		ASTExpressionNode* getDiffExpression(){
			return stepSize_;
		}
		ASTBlock *getForBody() {
			return block_;
		}
		Value *accept(Visitor *) override;
		

	private:
		string iterName_;
		ASTExpressionNode *initExpression_;
		ASTExpressionNode *finalExpression_;
		ASTExpressionNode *stepSize_;
		ASTBlock *block_;
};

class ASTWhileStatementDeclNode : public ASTStatementDeclNode {
	public:
		ASTWhileStatementDeclNode(ASTExpressionNode *e, ASTBlock* b): ASTStatementDeclNode(while_statement) {
			block_ = b;
			expr_ = e;
		}
		ASTExpressionNode* getExpression(){
			return expr_;
		}
		ASTBlock* getWhileBlock(){
			return block_;
		}
		Value *accept(Visitor *) override;

	private:
		ASTBlock* block_;
		ASTExpressionNode* expr_;
};

class ASTLabelDeclNode : public ASTStatementDeclNode {
	public:
		ASTLabelDeclNode(string l) : ASTStatementDeclNode(label_statement) {
			labelName_ = l;
		}
		virtual string get_label_name(){
			return labelName_;
		}
	private:
		string labelName_;
		int stmt_n0;
};

class ASTGotoDeclNode : public ASTStatementDeclNode {
	public:
		ASTGotoDeclNode(string l,ASTExpressionNode* cond): ASTStatementDeclNode(goto_statement){
			
			label_ = l;
			condition_ = cond;
		}
		ASTGotoDeclNode(string l): ASTStatementDeclNode(goto_statement){
			label_ = l;
			condition_ = NULL;
		}
		virtual string get_label_name(){
			return label_;
		}
		ASTExpressionNode* getExpression(){
			return condition_;
		}
		
		Value *accept(Visitor *) override;
	private:
		string label_;
		ASTExpressionNode* condition_;
};


class ASTPrintLitNode : ASTNode{
	public:
		ASTPrintLitNode(ASTExpressionNode *temp){
			isexpr_ = true;
			expr_ = temp;
		};
		ASTPrintLitNode(string temp){
			isexpr_ = false;
			str_lit_ = temp;
		};
		ASTPrintLitNode() : isexpr_(false) {}
		
		bool getType(){
			return isexpr_;
		}
		string getString(){
			return str_lit_;
		}
		ASTExpressionNode* getExpression(){
			return expr_;
		}
		Value *accept(Visitor *) override;
	private:
		bool isexpr_;
		ASTExpressionNode* expr_;
		string str_lit_;
};

class ASTPrintNode : public ASTStatementDeclNode{
	public:
		ASTPrintNode(): ASTStatementDeclNode(print_statement){}
		void setType(string temp){
			if(temp=="println")
				printNewline_ = true;
			else
				printNewline_ = false;
		}
		void push(ASTPrintLitNode* temp){
			(printList_).push_back(temp);
		}
		bool getType(){
			return printNewline_;
		}
		list<ASTPrintLitNode *>* getPrintList(){
			return &printList_;
		}
		Value *accept(Visitor *) override;
		
	private:
		bool printNewline_;
		list<ASTPrintLitNode *> printList_;
};


class ASTReadNode : public ASTStatementDeclNode{
	public:
		ASTReadNode(ASTLocationNode* l): ASTStatementDeclNode(read_statement){
			location_ = l;
		}
		ASTLocationNode* getLocation(){
			return location_;
		}
		Value *accept(Visitor *) override;
	private:
		ASTLocationNode* location_;
};


class Visitor {
	public:
		virtual Value* visit(ASTIntegerLiteralExpressionNode*) = 0;
		virtual Value* visit(ASTLocationNode*) = 0;
		virtual Value* visit(ASTAssignmentStatementNode*) = 0;
		virtual Value* visit(ASTBlock*) = 0;
		virtual Value* visit(ASTGotoDeclNode*) = 0;
		virtual Value* visit(ASTPrintNode* ) = 0;
		virtual Value* visit(ASTBinaryExpressionNode* ) = 0;
		virtual Value* visit(ASTIfStatementDeclNode* ) = 0;
		virtual Value* visit(ASTForStatementDeclNode* ) = 0;
		virtual Value* visit(ASTWhileStatementDeclNode* ) = 0;
		virtual Value* visit(ASTReadNode*) = 0;
		virtual Value* visit(ASTBoolLiteralExpressionNode*) = 0;
		virtual Value* visit(ASTLocationExpressionNode*) = 0;
		
		virtual Value* visit(ASTPrintLitNode*) = 0;
		virtual Value* visit(ASTStatementDeclListNode*) = 0;
		//virtual Value* visit(ASTParameterDecl*) = 0;
		//virtual Value* visit(ASTParameterDeclListNode*) = 0;
		//virtual Value* visit(ASTDeclBlockNode*) = 0;

		virtual Value* visit(ASTProgramNode*) = 0;

};


class Interpreter : public Visitor{

    public:
		Value* visit(ASTIntegerLiteralExpressionNode* temp);
		Value* visit(ASTLocationNode* node);
		Value* visit(ASTAssignmentStatementNode* node);
		Value* visit(ASTBlock* node);
		Value* visit(ASTGotoDeclNode* node);
		Value* visit(ASTPrintNode *node);
		Value* visit(ASTBinaryExpressionNode *node);
		Value* visit(ASTIfStatementDeclNode *node);
		Value* visit(ASTForStatementDeclNode *node);
		Value* visit(ASTWhileStatementDeclNode *node);
		Value* visit(ASTReadNode* node);
		Value* visit(ASTBoolLiteralExpressionNode* node);
		Value* visit(ASTLocationExpressionNode* node);

		Value* visit(ASTPrintLitNode *node);
		Value* visit(ASTStatementDeclListNode *node);
		//Value* visit(ASTParameterDecl *node);
		//Value* visit(ASTParameterDeclListNode *node);
		//Value* visit(ASTDeclBlockNode *node);
		Value* visit(ASTProgramNode *node);
};


class Symbol {
	public:
		Symbol(string id) : id_(id), literal_(NULL) {}
		Symbol(string id, ASTIntegerLiteralExpressionNode* lit) : id_(id), literal_(lit) {}

		string id_;
		ASTIntegerLiteralExpressionNode *literal_;
};


void annotateSymbolTable(int datatype, list<Symbol*> *VariableList);


