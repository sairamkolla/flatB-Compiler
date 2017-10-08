#include <iostream>
#include <stdlib.h>
#include <string>
#include <map>
#include <list>

using namespace std;

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
	label_statement = 5, goto_statement = 6, print_statement = 8, read_statement = 9};

class ASTNode {

};

class ASTExpressionNode : public ASTNode {
	public:
		ASTExpressionNode(exprType expr_type){
			expr_type_ = expr_type;
		}
	private:
		exprType expr_type_;
};


class ASTLocationNode: public ASTNode{
	public:
		ASTLocationNode(bool is_array){
			is_array_ = is_array;
		}
	private:
		bool is_array_;
};


class ASTVarLocationNode : public ASTLocationNode {
	public:
		ASTVarLocationNode(string id) : var_(id), ASTLocationNode(false) {}
		const string getVar() const {
			return var_;
		}

	private:
		string var_;
};

class ASTArrayLocationNode : public ASTLocationNode {
	public:
		ASTArrayLocationNode(string id, ASTExpressionNode *ex) : var_(id), expr_(ex), ASTLocationNode(true) {}
		const string getVar() const {
			return var_;
		}
		ASTExpressionNode *getExpression() const {
			return expr_;
		}

	private:
		string var_;
		ASTExpressionNode *expr_;
};



class ASTIntegerLiteralExpressionNode : public ASTExpressionNode{
	public:
		ASTIntegerLiteralExpressionNode(int value): ASTExpressionNode(literal){
			value_ = value;
		}
		int getValue(){
			return value_;
		}
	private:
		int value_;
};

class ASTBoolLiteralExpressionNode : public ASTExpressionNode {
	public:
		ASTBoolLiteralExpressionNode(string val) : ASTExpressionNode(literal) {
			value_ = val;
		}
		string getValue() const {
			return value_;
		}

	private:
		string value_;
};


class ASTLocationExpressionNode : public ASTExpressionNode {
	public:
		ASTLocationExpressionNode(ASTLocationNode *loc) : ASTExpressionNode(location) {
			location_ = loc;
		}
		ASTLocationNode *getLocation() const {
			return location_;
		}

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

	private:
		statementType statementId_;
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
	private:
		ASTLocationNode* location_;
		ASTExpressionNode* expr_;
		string operator_;
};




class ASTBlock : public ASTNode {
	public:
		ASTBlock(list<ASTStatementDeclNode *> *s) {
			statementList_ = s;
		}
		list<ASTStatementDeclNode *> *getStatementList() {
			return statementList_;
		}
	private:
		list<ASTStatementDeclNode *> *statementList_;
};

class ASTProgramNode : public ASTNode {
	public:
		ASTProgramNode(ASTBlock* code_block){
			code_block_ = code_block;
		}
	private:
		ASTBlock *code_block_;
};



class ASTIfStatementDeclNode : public ASTStatementDeclNode {
	public:
		ASTIfStatementDeclNode(ASTExpressionNode *ifExp, ASTBlock *ifBlock, ASTBlock *elseBlock) : ASTStatementDeclNode(if_statement) {
			ifExpression_ = ifExp;
			ifBlock_ = ifBlock;
			elseBlock_ = elseBlock;
		}
		ASTExpressionNode *getIfExpression() const {
			return ifExpression_;
		}
		ASTBlock *getIfBlock() const {
			return ifBlock_;
		}
		ASTBlock *getElseBlock() const {
			return elseBlock_;
		}

	private:
		ASTExpressionNode *ifExpression_;
		ASTBlock *ifBlock_;
		ASTBlock *elseBlock_;
};

class ASTForStatementDeclNode : public ASTStatementDeclNode {
	public:
		ASTForStatementDeclNode(string it, ASTExpressionNode *start, ASTExpressionNode *end, int d,ASTBlock *b) : ASTStatementDeclNode(for_statement) {
			iterName_ = it;
			initExpression_ = start;
			finalExpression_ = end;
			block_ = b;
			stepSize_ = d;
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
		ASTBlock *getForBody() {
			return block_;
		}

	private:
		string iterName_;
		ASTExpressionNode *initExpression_;
		ASTExpressionNode *finalExpression_;
		int stepSize_;
		ASTBlock *block_;
};

class ASTWhileStatementNode : public ASTStatementDeclNode {
	public:
		ASTWhileStatementNode(ASTExpressionNode *e, ASTBlock* b): ASTStatementDeclNode(while_statement) {
			block_ = b;
			expr_ = e;
		}
		ASTExpressionNode* getExpression(){
			return expr_;
		}
		ASTBlock* getBlock(){
			return block_;
		}
	private:
		ASTBlock* block_;
		ASTExpressionNode* expr_;
};

class ASTLabelDeclNode : public ASTStatementDeclNode {
	public:
		ASTLabelDeclNode(string l) : ASTStatementDeclNode(label_statement) {
			labelName_ = l;
		}
	private:
		string labelName_;
};

class ASTGotoDeclNode : public ASTStatementDeclNode {
	public:
		ASTGotoDeclNode(ASTLabelDeclNode* l,ASTExpressionNode* cond): ASTStatementDeclNode(goto_statement){
			label_ = l;
			condition_ = cond;
		}
		ASTLabelDeclNode* getLabel(){
			return label_;
		}
		ASTExpressionNode* getExpression(){
			return condition_;
		}
	private:
		ASTLabelDeclNode* label_;
		ASTExpressionNode* condition_;
};



class ASTPrintLitNode : public ASTNode{
	public:
		ASTPrintLitNode(bool isexpr) : isexpr_(isexpr) {}
	private:
		bool isexpr_;
};

class ASTPrintExprNode : public ASTPrintLitNode {
	public:
		ASTPrintExprNode(ASTExpressionNode *ex): ASTPrintLitNode(true) {
			expr_ = ex;
		}
		ASTExpressionNode *getExpression(){
			return expr_;
		}

	private:
		ASTExpressionNode *expr_;
};

class ASTPrintStringNode : public ASTPrintLitNode{
	public:
		ASTPrintStringNode(string s): ASTPrintLitNode(false){
			str_ = s;
		}
		string getString(){
			return str_;
		}
	private:
		string str_;
};

class ASTPrintNode : public ASTStatementDeclNode{
	public:
		ASTPrintNode(bool flag,list<ASTPrintLitNode *> *pl): ASTStatementDeclNode(print_statement){
			printNewline_ = flag;
			printList_ = pl;
		}
	private:
		bool printNewline_;
		list<ASTPrintLitNode *> *printList_;
};

class ASTReadNode : public ASTStatementDeclNode{
	public:
		ASTReadNode(ASTLocationNode* l): ASTStatementDeclNode(read_statement){
			location_ = l;
		}
		ASTLocationNode* getLocation(){
			return location_;
		}
	private:
		ASTLocationNode* location_;
};


class Symbol {
	public:
		Symbol(string id) : id_(id), literal_(NULL) {}
		Symbol(string id, ASTIntegerLiteralExpressionNode* lit) : id_(id), literal_(lit) {}

		string id_;
		ASTIntegerLiteralExpressionNode *literal_;
};


void annotateSymbolTable(int datatype, list<Symbol *> *variableList);

