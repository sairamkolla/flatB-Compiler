#include "head.h"
#include "ast.h"
#include "stdllvm.h"

using namespace std;
using namespace llvm;

static LLVMContext &Context = getGlobalContext();
Module *flatBToLLVM = new Module("flatBToLLVM", getGlobalContext());
IRBuilder<> *Builder = new IRBuilder<>(getGlobalContext());
static map<string, Value *> symTable;
vector<string> FunArgs;
Function *fooFunc;
BasicBlock *entry;
BasicBlock* cur_block;

void Error(const char *S) {
	cout << S << endl;
}
Value *ErrorV(const char *Str) {
	Error(Str);
	return nullptr;
}


Function *createFunc(string name) {
	std::vector<Type *> Integers(FunArgs.size(), Type::getInt32Ty(Context));
	FunctionType *funcType = llvm::FunctionType::get(Builder->getInt32Ty(), Integers, false);
	Function* fooFunc = llvm::Function::Create(
	                        funcType, llvm::Function::ExternalLinkage, name, flatBToLLVM);
	return fooFunc;
}

BasicBlock *createBB(Function *foofunc, string name) {
	return BasicBlock::Create(Context, name, foofunc);
}

void setFuncArgs(Function* fooFunc, vector<string> FunArgs) {
	unsigned Idx = 0;
	Function::arg_iterator AI, AE;
	for (AI = fooFunc->arg_begin(), AE = fooFunc->arg_end(); AI != AE; ++AI, ++Idx) {
		AI->setName(FunArgs[Idx]);
	}
}


void CreateFooFunction() {
	fooFunc = createFunc("main");
	entry = createBB(fooFunc, "entry");
	Builder->SetInsertPoint(entry);
}

Value* Interpreter::visit(ASTIntegerLiteralExpressionNode* node) {
	//	cout << "returning " << node->getValue() << endl;
	return Builder->getInt32(node->getValue());
}
Value* Interpreter::visit(ASTLocationNode* node) {
	bool is_array = node->getType();
	string var;
	if (!is_array) {
		var = node->getVarName();
		/*
		cout << "variable name is" << var << endl;
		if (symTable.find(var) != symTable.end())
			cout << "variable is there in symbol table" << endl;
		*/
		cout << "returned location of " << var << endl;
		return symTable[var];
	} else {
		var = node->getVarName();
		Value *ret = symTable[var];
		Value *size = node->getExpression()->accept(this);
		if (size->getType()->isPointerTy()) {
			size = Builder->CreateLoad(size, "tmp");
		}
		vector<Value*> v;
		v.push_back(Builder->getInt64(0));
		v.push_back(Builder->CreateSExt(size, Builder->getInt64Ty(), "zext"));
		ArrayRef<Value *> a = ArrayRef<Value *>(v);
		return Builder->CreateInBoundsGEP(ret, v, "getptr");
	}
}



Value* Interpreter::visit(ASTAssignmentStatementNode* node) {
	Value *ptr = node->getLocation()->accept(this);
	Value *val = node->getExpression()->accept(this);
	string op = node->getOperator();
	if (val->getType()->isPointerTy()) {
		val = Builder->CreateLoad(val, "tmp");
		cout << "loadedd" << endl;
	}
	Builder->CreateStore(val, ptr, false);
	return Builder->getInt32(0);
}
Value* Interpreter::visit(ASTBlock* node) {
	list<ASTStatementDeclNode *> *s = node->getStatementList()->getStatementList();
	list<ASTStatementDeclNode *>::iterator it;

	Value *v;
	for (it = s->begin(); it != s->end(); it++) {
		v = (*it)->accept(this);
		if (!v) {
			cout << "recevied an error " << endl;				// If the return value is nullptr, then
			break;					// do not process further statements.
		} else {
			cout << "one statement done" << endl;
		}
	}
	//cout << "All statements processde in the block" << endl;
	return v;
}



Value* Interpreter::visit(ASTProgramNode* node) {
	CreateFooFunction();
	cur_block = entry;
	Value *v =  node->getCodeBlock()->accept(this);
	Builder->CreateRet(Builder->getInt32(0));
	cout << "Code Generation Successfull" << endl;
	return v;
}




Value* Interpreter::visit(ASTGotoDeclNode* node) {
	return nullptr;
}
Value* Interpreter::visit(ASTPrintNode *node) {

	//cout << "into the print node" << endl;
	list<ASTPrintLitNode*> *print_list = node->getPrintList();
	list<ASTPrintLitNode*>::reverse_iterator it;
	vector<Value*> argsV;

	string to_print;
	argsV.push_back((llvm::Value *)NULL);



	for (it = (*print_list).rbegin(); it != (*print_list).rend(); it++) {
		ASTPrintLitNode* temp = *it;
		Value *arg = (*it)->accept(this);
		argsV.push_back(arg);
		if (arg->getType() == llvm::IntegerType::getInt32Ty(Context))  // It's a expression
			to_print += "%d";
		else
			to_print += "%s";

		//cout << "print lit created" << endl;
		//cout << "args size is " << argsV.size() << endl;
	}
	//cout << "out of print lits loop"  << endl;
	if (node->getType())
		to_print += "\n";

	//cout << "Setting format as " <<  to_print << endl;
	argsV[0] = Builder->CreateGlobalStringPtr(to_print.c_str());

	//cout << "print node finished" << endl;

	Function *printf_func = flatBToLLVM->getFunction("printf");
	if (!printf_func) {
		printf_func = llvm::Function::Create(llvm::FunctionType::get(llvm::IntegerType::getInt32Ty(Context), true), llvm::GlobalValue::ExternalLinkage, "printf", flatBToLLVM);
		printf_func->setCallingConv(llvm::CallingConv::C);
		llvm::AttributeSet printf_PAL;
		printf_func->setAttributes(printf_PAL);
	}

	//cout << "print function loaded" << endl;
	return Builder->CreateCall(printf_func, argsV);
}

Value* Interpreter::visit(ASTPrintLitNode *node) {
	if (node->getType()) { // It's a expression
		//cout << "got an expression" << endl;
		Value* v = node->getExpression()->accept(this);

		if (v->getType()->isPointerTy()) {
			//exp = Builder->CreateLoad(exp, "loadarg");
			v = Builder->CreateLoad(v);
		}

		return v;
	} else {
		//cout << "got a string" << endl;
		string s = node->getString();
		s = s.substr(1, s.size() - 2);
		//cout << "string is " << s << endl;

		Value *v = Builder->CreateGlobalStringPtr(s.c_str());
		return v;
	}
}
Value* Interpreter::visit(ASTBinaryExpressionNode *node) {
	Value *L = node->getLHS()->accept(this);
	Value *R = node->getRHS()->accept(this);
	if (!L || !R) {
		return nullptr;
	}
	if (L->getType()->isPointerTy()) {
		L = Builder->CreateLoad(L, "tmp");
	}
	if (R->getType()->isPointerTy()) {
		R = Builder->CreateLoad(R, "tmp");
	}
	int op = node->getOperatorId();
	//cout << "Operation id is " << op << endl;
	switch (op) {
	case _plus:
		return Builder->CreateAdd(L, R, "ADD");
	case _minus:
		return Builder->CreateSub(L, R, "SUB");
	case _mult:
		return Builder->CreateMul(L, R, "MUL");
	case _div:
		return Builder->CreateUDiv(L, R, "DIV");
	case _mod:
		return Builder->CreateURem(L, R, "MOD");
	case _and:
		return Builder->CreateAnd(L, R, "AND");
	case _or:
		return Builder->CreateOr(L, R, "OR");
	case _eq: {
		cout << "checking for equality" << endl;
		return Builder->CreateICmpEQ(L, R, "EQ");
	}
	case _neq:
		return Builder->CreateICmpNE(L, R, "NEQ");
	case _lt:
		return Builder->CreateICmpSLT(L, R, "LT");
	case _gt:
		return Builder->CreateICmpSGT(L, R, "GT");
	case _lteq:
		return Builder->CreateICmpSLE(L, R, "LTEQ");
	case _gteq:
		return Builder->CreateICmpSGE(L, R, "GTEQ");
	default:
		return ErrorV("Invalid Binary Operator");
	}
	return nullptr;

}
Value* Interpreter::visit(ASTIfStatementDeclNode *node) {
	ASTExpressionNode *ifExp = node->getExpression();
	ASTBlock *ifBlock = node->getIfBlock();
	ASTBlock *elseBlock = node->getElseBlock();
	Value *v = ifExp->accept(this);
	Value *ifVal = Builder->getInt32(1);
	Value *elseVal = Builder->getInt32(1);

	BasicBlock *ifb, *elseb, *conb, *afterb;

	if (!v) {
		return nullptr;
	}

	if (v->getType()->isIntegerTy(32)) {
		v = Builder->CreateICmpEQ(v, Builder->getInt32(1), "cmp");
	}
	else {
		v = Builder->CreateICmpEQ(v, Builder->getInt1(1), "cmp");
	}
	if (elseBlock) {
		conb = BasicBlock::Create(Context, "check condition", cur_block->getParent());
		ifb = BasicBlock::Create(Context, "if block", cur_block->getParent());
		elseb = BasicBlock::Create(Context, "else block", cur_block->getParent());
		afterb = BasicBlock::Create(Context, "after block", cur_block->getParent());

		Builder->CreateBr(conb);

		Builder->SetInsertPoint(conb);
		cur_block = conb;
		Value *v = ifExp->accept(this);

		if (!v) {
			return nullptr;
		}
		if (v->getType()->isIntegerTy()) {
			if (v->getType()->isIntegerTy(32)) {
				v = Builder->CreateICmpEQ(v, Builder->getInt32(1), "cmp");
			}
			else {
				v = Builder->CreateICmpEQ(v, Builder->getInt1(1), "cmp");
			}
			Builder->CreateCondBr(v, ifb, elseb);
		}

		// If block
		Builder->SetInsertPoint(ifb);
		cur_block = ifb;
		ifBlock->accept(this);
		Builder->CreateBr(afterb);

		// Else Block
		Builder->SetInsertPoint(elseb);
		cur_block = elseb;
		elseBlock->accept(this);
		Builder->CreateBr(afterb);

		//After Block
		Builder->SetInsertPoint(afterb);
		cur_block = afterb;
		return Builder->getInt32(0);


	}
	else {
		conb = BasicBlock::Create(Context, "check condition", cur_block->getParent());
		ifb = BasicBlock::Create(Context, "if block", cur_block->getParent());
		afterb = BasicBlock::Create(Context, "after block", cur_block->getParent());

		Builder->CreateBr(conb);

		Builder->SetInsertPoint(conb);
		cur_block = conb;
		Value *v = ifExp->accept(this);

		if (!v) {
			return nullptr;
		}
		if (v->getType()->isIntegerTy()) {
			if (v->getType()->isIntegerTy(32)) {
				v = Builder->CreateICmpEQ(v, Builder->getInt32(1), "cmp");
			}
			else {
				v = Builder->CreateICmpEQ(v, Builder->getInt1(1), "cmp");
			}
			Builder->CreateCondBr(v, ifb, afterb);
		}

		// If block
		Builder->SetInsertPoint(ifb);
		cur_block = ifb;
		ifBlock->accept(this);
		Builder->CreateBr(afterb);

		//After Block
		Builder->SetInsertPoint(afterb);
		cur_block = afterb;
		return Builder->getInt32(0);
	}



	


	return nullptr;
}
Value* Interpreter::visit(ASTForStatementDeclNode *node) {
	ASTExpressionNode *start = node->getInitExpression();
	ASTExpressionNode *end = node->getFinalExpression();
	ASTExpressionNode *diff = node->getDiffExpression();
	string it = node->getIterVarName();
	ASTBlock *ForBody = node->getForBody();


	//Initialization
	Value* var = symTable[it];
	Value* init = start->accept(this);
	if (init->getType()->isPointerTy()) {
		init = Builder->CreateLoad(init);
	}
	Builder->CreateStore(init, var, false);


	llvm::BasicBlock *loop_block, *check_block, *after_block;
	check_block = BasicBlock::Create(Context, "check condition", cur_block->getParent());
	loop_block = BasicBlock::Create(Context, "loop", cur_block->getParent());
	after_block = BasicBlock::Create(Context, "Afterloop", cur_block->getParent());


	//Check block
	Builder->CreateBr(check_block);
	Builder->SetInsertPoint(check_block);
	cur_block = check_block;
	Value *v = symTable[it];
	Value *endVal = end->accept(this);

	if (v->getType()->isPointerTy()) {
		v = Builder->CreateLoad(v);
	}
	if (endVal->getType()->isPointerTy()) {
		endVal = Builder->CreateLoad(endVal);
	}

	endVal = Builder->CreateICmpSGE(endVal, v, "loopcond");
	Builder->CreateCondBr(endVal, loop_block, after_block);


	//cout << "check block done" << endl;

	//Loop Block;
	Builder->SetInsertPoint(loop_block);
	cur_block = loop_block;
	Value *x = ForBody->accept(this);
	if (x != nullptr) {
		Value* diff_value = diff->accept(this);
		if (diff_value->getType()->isPointerTy()) {
			diff_value = Builder->CreateLoad(diff_value);
		}
		v = symTable[it];
		if (v->getType()->isPointerTy()) {
			v = Builder->CreateLoad(v);
		}

		Value *next_v = Builder->CreateAdd(v, diff_value, "ADD");
		v = symTable[it];
		Builder->CreateStore(next_v, v, false);
		Builder->CreateBr(check_block);
	}
	//cout << "loop block done" << endl;

	//After the loop block
	Builder->SetInsertPoint(after_block);
	cur_block = after_block;
	return Builder->getInt32(0);


	/*
		loop *thisLoop = (loop *)malloc(sizeof(loop));
		thisLoop->entryBB = LoopBB;
		thisLoop->afterBB = AfterBB;
		thisLoop->var_ = var;
		thisLoop->endExp = end;
		loops.push(thisLoop);
		loops.pop();
		*/
}
Value* Interpreter::visit(ASTWhileStatementDeclNode *node) {
	cout << "into the while loop" << endl;
	ASTExpressionNode *whileExp = node->getExpression();
	ASTBlock *whileBlock = node->getWhileBlock();
	llvm::BasicBlock *loop_block, *check_block, *after_block;
	check_block = BasicBlock::Create(Context, "check condition", cur_block->getParent());
	loop_block = BasicBlock::Create(Context, "loop", cur_block->getParent());
	after_block = BasicBlock::Create(Context, "Afterloop", cur_block->getParent());

	Builder->CreateBr(check_block);
	Builder->SetInsertPoint(check_block);
	cur_block = check_block;
	Value *v = whileExp->accept(this);
	if (!v) {
		return nullptr;
	}
	if (v->getType()->isIntegerTy()) {
		if (v->getType()->isIntegerTy(32)) {
			v = Builder->CreateICmpEQ(v, Builder->getInt32(1), "cmp");
		}
		else {
			v = Builder->CreateICmpEQ(v, Builder->getInt1(1), "cmp");
		}
		Builder->CreateCondBr(v, loop_block, after_block);

	}
	cout << "check block done" << endl;

	Builder->SetInsertPoint(loop_block);
	cur_block = loop_block;
	Value *x = whileBlock->accept(this);

	if (x != nullptr) {
		Builder->CreateBr(check_block);
	}
	cout << "loop block done" << endl;
	Builder->SetInsertPoint(after_block);
	cur_block = after_block;
	return Builder->getInt32(0);

}
Value* Interpreter::visit(ASTReadNode * node) {
	vector<Value* > args;
	string to_read = "%d";
	Value* loc  = node->getLocation()->accept(this);

	llvm::Function *scanf_func = flatBToLLVM->getFunction("scanf");

	if (!scanf_func) {
		scanf_func = llvm::Function::Create(llvm::FunctionType::get(llvm::IntegerType::getInt32Ty(Context), true), llvm::GlobalValue::ExternalLinkage, "scanf", flatBToLLVM);
		scanf_func->setCallingConv(llvm::CallingConv::C);
		llvm::AttributeSet printf_PAL;
		scanf_func->setAttributes(printf_PAL);
	}
	args.push_back(Builder->CreateGlobalStringPtr(to_read.c_str()));
	args.push_back(loc);
	return Builder->CreateCall(scanf_func, args);
}
Value* Interpreter::visit(ASTStatementDeclListNode * node) {
	return nullptr;
}

Value* Interpreter::visit(ASTBoolLiteralExpressionNode * node) {
	cout << "returning " << node->getValue() << endl;
	if (node->getValue() == true)
		return Builder->getInt1(1);
	return Builder->getInt1(0);
}
Value* Interpreter::visit(ASTLocationExpressionNode * node) {
	Value *x = node->getLocation()->accept(this);
	return x;
	return nullptr;
}


Type *getLLVMType(int decafTy) {
	switch (decafTy) {
	case _int_:
		return Builder->getInt32Ty();
	case _bool_:
		return Builder->getInt1Ty();
	default:
		runtime_error("Unknown Datatype");
	}
}

void annotateSymbolTable(int datatype, list<Symbol*> *VariableList) {

	list<Symbol*>::iterator iter;
	Interpreter v_;
	bool isArray = false;
	for (iter = VariableList->begin(); iter != VariableList->end(); iter++) {
		Symbol *sym = *iter;
		Value *v = nullptr;
		if (sym->literal_ != 0) {
			v = sym->literal_->accept(&v_);
			isArray = true;

		}
		ConstantInt *c;
		if (v) {
			c = dyn_cast<ConstantInt>(v);
		}
		Type *ty = getLLVMType(datatype);
		GlobalVariable *var;
		if (!isArray) {
			var = new GlobalVariable(*flatBToLLVM, ty, false, GlobalValue::CommonLinkage,
			                         0, sym->id_.c_str());
			var->setAlignment(4);
			if (ty->isIntegerTy(32)) {
				var->setInitializer(Builder->getInt32(0));
			}
			else {
				var->setInitializer(Builder->getInt1(0));
			}
		}
		else {
			ArrayType* ArrayTy_0 = ArrayType::get(ty, c->getSExtValue());
			PointerType* PointerTy_1 = PointerType::get(ArrayTy_0, 0);
			var = new GlobalVariable(*flatBToLLVM, ArrayTy_0, false, GlobalValue::CommonLinkage,
			                         0, sym->id_.c_str());
			var->setAlignment(16);
			ConstantAggregateZero* const_array_2 = ConstantAggregateZero::get(ArrayTy_0);
			var->setInitializer(const_array_2);
		}
		symTable.insert(make_pair(sym->id_, var));
	}


}



/* Accepter dispatch functions for visitor design patter */




Value* ASTIntegerLiteralExpressionNode::accept( Visitor * v) {
	return v->visit(this);
}
Value* ASTLocationNode::accept( Visitor * v) {
	return v->visit(this);
}
Value* ASTAssignmentStatementNode::accept( Visitor * v) {
	return v->visit(this);
}
Value* ASTBlock::accept( Visitor * v) {
	return v->visit(this);
}
Value* ASTGotoDeclNode::accept( Visitor * v) {
	return v->visit(this);
}
Value* ASTPrintNode::accept( Visitor * v) {
	return v->visit(this);
}
Value* ASTPrintLitNode::accept( Visitor * v) {
	return v->visit(this);
}
Value* ASTBinaryExpressionNode::accept( Visitor * v) {
	return v->visit(this);
}
Value* ASTIfStatementDeclNode::accept( Visitor * v) {
	return v->visit(this);
}
Value* ASTForStatementDeclNode::accept( Visitor * v) {
	return v->visit(this);
}
Value* ASTWhileStatementDeclNode::accept( Visitor * v) {
	return v->visit(this);
}
Value* ASTReadNode::accept( Visitor * v) {
	return v->visit(this);
}
Value* ASTStatementDeclListNode::accept( Visitor * v) {
	return v->visit(this);
}

Value* ASTBoolLiteralExpressionNode::accept(Visitor * v) {
	return v->visit(this);
}
Value* ASTLocationExpressionNode::accept(Visitor * v) {
	return v->visit(this);
}

Value* ASTProgramNode::accept(Visitor * v) {
	return v->visit(this);
}
