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
extern unordered_map<string,int> vardict;
extern unordered_map<string,int*> arrdict;
extern int var_type[9999];
extern unordered_map<string,int> var_index;

enum exprType { binary = 1, location = 2, literal = 3 , Unary = 4};
enum statementType { assign_statement = 1, for_statement = 2, if_statement = 3, while_statement = 4, 
	label_statement = 5, goto_statement = 6, print_statement = 8, read_statement = 9,default_statement=0};

class Visitor;
class Interpreter;
class ASTNode;
class ASTParameterDecl;
class ASTExpressionNode;
class ASTLocationNode;
class ASTVarLocationNode;
class ASTArrayLocationNode;
class ASTIntegerLiteralExpressionNode;
class ASTBoolLiteralExpressionNode;
class ASTLocationExpressionNode;
class ASTBinaryExpressionNode;
class ASTUnaryExpressionNode;
class ASTStatementDeclNode;
class ASTAssignmentStatementNode;
class ASTBlock;
class ASTProgramNode;
class ASTIfStatementDeclNode;
class ASTForStatementDeclNode;
class ASTWhileStatementNode;
class ASTLabelDeclNode;
class ASTGotoDeclNode;
class ASTPrintLitNode;
//class ASTPrintExprNode;
//class ASTPrintStringNode;
class ASTPrintNode;
class ASTReadNode;
class ASTStatementDeclListNode;

class Visitor{
	public:
		virtual int visit(ASTNode* temp){}
		virtual int visit(ASTParameterDecl* temp){}
		virtual int visit(ASTExpressionNode* temp){}
		virtual int visit(ASTLocationNode* temp){}
		virtual int visit(ASTVarLocationNode* temp){}
		virtual int visit(ASTArrayLocationNode* temp){}
		virtual int visit(ASTIntegerLiteralExpressionNode* temp){}
		virtual int visit(ASTBoolLiteralExpressionNode* temp){}
		virtual int visit(ASTLocationExpressionNode* temp){}
		virtual int visit(ASTBinaryExpressionNode* temp){}
		virtual int visit(ASTUnaryExpressionNode* temp){}
		virtual int visit(ASTStatementDeclNode* temp){}
		virtual int visit(ASTAssignmentStatementNode* temp){}
		virtual int visit(ASTBlock* temp){}
		virtual int visit(ASTProgramNode* temp){}
		virtual int visit(ASTIfStatementDeclNode* temp){}
		virtual int visit(ASTForStatementDeclNode* temp){}
		virtual int visit(ASTWhileStatementNode* temp){}
		virtual int visit(ASTLabelDeclNode* temp){}
		virtual int visit(ASTGotoDeclNode* temp){}
		virtual int visit(ASTPrintLitNode* temp){}
		//virtual int visit(ASTPrintExprNode* temp){}
		//virtual int visit(ASTPrintStringNode* temp){}
		virtual int visit(ASTPrintNode* temp){}
		virtual int visit(ASTReadNode* temp){}
};


class ASTNode {
public:
	ASTNode(){

	};
	virtual int accept(Visitor* v)=0;
};
/*
class ASTParameterDecl : public ASTNode {
	public:
		ASTParameterDecl(int t, string name, bool arr) {
			type_ = t;
			varName_ = name;
			isArray_ = arr;
		}
		const int getType() const {
			return type_;
		}
		string getVarName() {
			return varName_;
		}
		const bool getIfArray() const {
			return isArray_;
		}

	private:
		int type_;
		string varName_;
		bool isArray_;
};
*/



class ASTExpressionNode : public ASTNode {
	public:
		ASTExpressionNode(exprType expr_type){
			expr_type_ = expr_type;
		}
		virtual int accept(Visitor* v){
			return v->visit(this);
		}
	private:
		exprType expr_type_;
};

class ASTVarLocationNode : public ASTNode {
	public:
		ASTVarLocationNode(string id) : var_(id) {}
		string getVarName() const {
			return var_;
		}
		int getType(){
			return 0;
		}
		virtual int accept(Visitor* v){
			return v->visit(this);
		};
	private:
		string var_;
};

class ASTArrayLocationNode : public ASTNode {
	public:
		ASTArrayLocationNode(string id, ASTExpressionNode *ex) : var_(id), expr_(ex) {}
		const string getVarName() const {
			return var_;
		}
		ASTExpressionNode *getExpression() const {
			return expr_;
		}
		int getType(){
			return 1;
		}
		virtual int accept(Visitor* v){
			return v->visit(this);
		};
	private:
		string var_;
		ASTExpressionNode *expr_;
};



class ASTLocationNode: public ASTNode{
	
	public:
		ASTLocationNode(ASTArrayLocationNode* temp){
			is_array_ = true;
			arr_ = temp;
			var_ = temp->getVarName();
		}
		ASTLocationNode(string temp){
			var_ = temp;
			is_array_ = false;
		}
		string getVarName(){
			return var_;
		}
		ASTArrayLocationNode* getArray(){
			return arr_;
		}
	private:
		bool is_array_;
		ASTArrayLocationNode* arr_;
		string var_;
};



class ASTIntegerLiteralExpressionNode : public ASTExpressionNode{
	public:
		ASTIntegerLiteralExpressionNode(int value): ASTExpressionNode(literal){
			value_ = value;
		}
		int getValue(){
			return value_;
		}
		virtual int accept(Visitor* v){
			return v->visit(this);
		};
	private:
		int value_;
};

class ASTBoolLiteralExpressionNode : public ASTExpressionNode {
	public:
		ASTBoolLiteralExpressionNode(bool val) : ASTExpressionNode(literal) {
			value_ = val;
		}
		bool getValue() const {
			return value_;
		}
		virtual int accept(Visitor* v){
			return v->visit(this);
		};

	private:
		bool value_;
};

/*
class ASTLocationExpressionNode : public ASTExpressionNode {
	public:
		ASTLocationExpressionNode(ASTLocationNode *loc) : ASTExpressionNode(location) {
			location_ = loc;
		}
		ASTLocationNode *getLocation() const {
			return location_;
		}
		virtual int accept(Visitor* v){
			return v->visit(this);
		};

	private:
		ASTLocationNode *location_;
};
*/



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
		ASTStatementDeclNode() : statementId_(default_statement){}
		virtual int accept(Visitor* v){
			return v->visit(this);
		};
	private:
		statementType statementId_;
};

class ASTStatementDeclListNode : public ASTNode{
	public:
		ASTStatementDeclListNode(){
		
		}

		void push(ASTStatementDeclNode* temp){
			stmts_->push_back(temp);
		}
		list<ASTStatementDeclNode *> *getStatementList() {
			return stmts_;
		}
	private:
		list<ASTStatementDeclNode *> *stmts_;
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
		virtual int accept(Visitor* v){
			return v->visit(this);
		};
	private:
		ASTLocationNode* location_;
		ASTExpressionNode* expr_;
		string operator_;
};



class ASTBlock : public ASTNode {
	public:
		ASTBlock(ASTStatementDeclListNode *temp) {
			statementList_ = temp;
		}
		ASTStatementDeclListNode *getStatementList() {
			return statementList_;
		}

		ASTStatementDeclListNode *statementList_;
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


/*
class ASTPrintLitNode : public ASTNode{
	public:
		ASTPrintLitNode(bool isexpr) : isexpr_(isexpr) {}
		ASTPrintLitNode() : isexpr_(false) {}
		virtual int accept(Visitor* v){
			return v->visit(this);
		};
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
		virtual int accept(Visitor* v){
			return v->visit(this);
		};

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
		virtual int accept(Visitor* v){
			return v->visit(this);
		};
	private:
		string str_;
};
*/

class ASTPrintLitNode : public ASTNode{
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
		virtual int accept(Visitor* v){
			return v->visit(this);
		};
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
			printList_->push_back(temp);
		}
		virtual int accept(Visitor* v){
			return v->visit(this);
		};
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
		virtual int accept(Visitor* v){
			return v->visit(this);
		};
	private:
		ASTLocationNode* location_;
};


class Interpreter:public Visitor{

    public:
        Interpreter()
        {

        };
		int visit(ASTIntegerLiteralExpressionNode* temp){
			return temp->getValue();
		}
		int visit(ASTVarLocationNode* temp){
			return vardict[temp->getVarName()];
		}
		int visit(ASTArrayLocationNode* temp){
			int index = temp->getExpression()->accept(this);
			string varname = temp->getVarName();
			return arrdict[varname][index];
		}
		int visit(ASTAssignmentStatementNode* temp){
			string varname = temp->getLocation()->getVarName();
			int value = temp->getExpression()->accept(this);
			if(var_type[var_index[varname]] == 1){ // Variable
				vardict[varname] == value;
			}else if(var_type[var_index[varname]] == 2){ //Array
				int index = temp->getLocation()->getArray()->getExpression()->accept(this);
				arrdict[varname][index] = value;
			}
			return 0;
		}
		int visit(ASTBlock* temp){
			ASTStatementDeclListNode* temp2 = temp->getStatementList();
		}

		

};


class Symbol {
	public:
		Symbol(string id) : id_(id), literal_(NULL) {}
		Symbol(string id, ASTIntegerLiteralExpressionNode* lit) : id_(id), literal_(lit) {}

		string id_;
		ASTIntegerLiteralExpressionNode *literal_;
};


void annotateSymbolTable(int datatype, list<Symbol *> *variableList);


