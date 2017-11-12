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
	cout << "returning " << node->getValue() << endl;
	return Builder->getInt32(node->getValue());
}
Value* Interpreter::visit(ASTLocationNode* node) {
	bool is_array = node->getType();
	string var;
	if (!is_array) {
		var = node->getVarName();
		cout << "variable name is" << var << endl;
		if (symTable.find(var) != symTable.end())
			cout << "variable is there in symbol table" << endl;

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
	if (val->getType()->isPointerTy())
		val = Builder->CreateLoad(val, "tmp");
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
		}else{
			cout << "one statement done" << endl;	
		}
	}
	cout << "All statements processde in the block" << endl;
	return v;
}



Value* Interpreter::visit(ASTProgramNode* node) {
	CreateFooFunction();
	Value *v =  node->getCodeBlock()->accept(this);
	Builder->CreateRet(Builder->getInt32(0));
	return v;
}




Value* Interpreter::visit(ASTGotoDeclNode* node) {
	return nullptr;
}
Value* Interpreter::visit(ASTPrintNode *node) {

	/*
	cout << "into the print node" << endl;
	list<ASTPrintLitNode*> *print_list = node->getPrintList();
	list<ASTPrintLitNode*>::reverse_iterator it;
	vector<Value*> argsV;
	vector<Type*> argTypes;

	for (it = (*print_list).rbegin(); it != (*print_list).rend(); it++) {
		ASTPrintLitNode* temp = *it;
		Value *arg = (*it)->accept(this);

		argsV.push_back(arg);

		argTypes.push_back(arg->getType());
		cout << "print lit created" << endl;

	}


	llvm::ArrayRef<Type*> typeargs(argTypes);
	llvm::ArrayRef<Value*> refargs(argsV);
	llvm::FunctionType *FType = FunctionType::get(Type::getInt32Ty(Context), typeargs, false);
	Constant* printfunc1 = flatBToLLVM->getOrInsertFunction("printf", FType);
	return Builder->CreateCall(printfunc1, refargs);
	*/

	cout << "into the print node" << endl;
	list<ASTPrintLitNode*> *print_list = node->getPrintList();
	list<ASTPrintLitNode*>::reverse_iterator it;
	vector<Value*> argsV;

	string to_print;
	argsV.push_back((llvm::Value *)NULL);



	for (it = (*print_list).rbegin(); it != (*print_list).rend(); it++) {
		ASTPrintLitNode* temp = *it;
		Value *arg = (*it)->accept(this);
		cout << "sdfsdfsdf" << endl;
		argsV.push_back(arg);
		cout << "111" << endl;
		if (arg->getType() == llvm::IntegerType::getInt32Ty(Context))  // It's a expression
			to_print += "%d";
		else
			to_print = "%s";

		cout << "print lit created" << endl;

	}
	if (node->getType())
		to_print += "\n";

	argsV[0] = Builder->CreateGlobalStringPtr(to_print.c_str());



	Function *printf_func = flatBToLLVM->getFunction("printf");
	if (!printf_func) {
		printf_func = llvm::Function::Create(llvm::FunctionType::get(llvm::IntegerType::getInt32Ty(Context), true), llvm::GlobalValue::ExternalLinkage, "printf", flatBToLLVM);
		printf_func->setCallingConv(llvm::CallingConv::C);
		llvm::AttributeSet printf_PAL;
		printf_func->setAttributes(printf_PAL);
	}

	return Builder->CreateCall(printf_func, argsV);
}

Value* Interpreter::visit(ASTPrintLitNode *node) {
	if (node->getType()) { // It's a expression
		cout << "got an expression" << endl;
		Value* v = node->getExpression()->accept(this);

		if (v->getType()->isPointerTy()) {
			//exp = Builder->CreateLoad(exp, "loadarg");
			v = Builder->CreateLoad(v);
		}
		/*else {
			if (v->getType()->isIntegerTy(32)) {
				v = Builder->CreateICmpEQ(v, Builder->getInt32(1), "cmp");
			}
			else if (v->getType()->isIntegerTy(1)) {
				v = Builder->CreateICmpEQ(v, Builder->getInt1(1), "cmp");
			}
		}*/
		return v;
	} else {
		cout << "got a string" << endl;
		string s = node->getString();
		s = s.substr(1, s.size() - 2);
		cout << "string is " << s << endl;

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
	cout << "Operation id is " << op << endl;
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
		Function *F = Builder->GetInsertBlock()->getParent();
		BasicBlock *ThenBB = BasicBlock::Create(getGlobalContext(), "then", F);
		if (elseBlock) {
			BasicBlock *ElseBB = BasicBlock::Create(getGlobalContext(), "else");
			BasicBlock *MergeBB = BasicBlock::Create(getGlobalContext(), "ifcont");
			Builder->CreateCondBr(v, ThenBB, ElseBB);
			Builder->SetInsertPoint(ThenBB);

			ifVal = ifBlock->accept(this);
			if (ifVal) {
				Builder->CreateBr(MergeBB);
			}
			ThenBB = Builder->GetInsertBlock();

			F->getBasicBlockList().push_back(ElseBB);
			Builder->SetInsertPoint(ElseBB);

			elseVal = elseBlock->accept(this);
			if (elseVal) {
				Builder->CreateBr(MergeBB);
			}
			ElseBB = Builder->GetInsertBlock();

			if (!ifVal && !elseVal) {
				return Builder->getInt32(0);
			}
			else {
				F->getBasicBlockList().push_back(MergeBB);
				Builder->SetInsertPoint(MergeBB);
				return Builder->getInt32(0);
			}
		}
		else {
			BasicBlock *MergeBB = BasicBlock::Create(getGlobalContext(), "ifcont");
			Builder->CreateCondBr(v, ThenBB, MergeBB);
			Builder->SetInsertPoint(ThenBB);

			ifVal = ifBlock->accept(this);
			if (ifVal) {
				Builder->CreateBr(MergeBB);
			}
			ThenBB = Builder->GetInsertBlock();

			F->getBasicBlockList().push_back(MergeBB);
			Builder->SetInsertPoint(MergeBB);

			return Builder->getInt32(0);
		}
	}




	return nullptr;
}
Value* Interpreter::visit(ASTForStatementDeclNode *node) {
	ASTExpressionNode *start = node->getInitExpression();
	ASTExpressionNode *end = node->getFinalExpression();
	ASTExpressionNode *diff = node->getDiffExpression();

	string it = node->getIterVarName();
	ASTBlock *body = node->getForBody();

	Value *init = start->accept(this);
	Function *F = Builder->GetInsertBlock()->getParent();

	BasicBlock *LoopBB = BasicBlock::Create(getGlobalContext(), "loop", F);
	BasicBlock *AfterBB = BasicBlock::Create(getGlobalContext(), "afterloop", F);

	BasicBlock *PreHeaderBB = Builder->GetInsertBlock();
	Builder->CreateBr(LoopBB);

	Builder->SetInsertPoint(LoopBB);
	PHINode *var = Builder->CreatePHI(Type::getInt32Ty(getGlobalContext()), 2, it.c_str());
	/*
	loop *thisLoop = (loop *)malloc(sizeof(loop));
	thisLoop->entryBB = LoopBB;
	thisLoop->afterBB = AfterBB;
	thisLoop->var_ = var;
	thisLoop->endExp = end;
	loops.push(thisLoop);
	*/
	var->addIncoming(init, PreHeaderBB);

	symTable[it] = var;

	Value *bodyVal = body->accept(this);
	if (bodyVal != nullptr) {
		Value* diff_value = diff->accept(this);
		if (diff_value->getType()->isPointerTy()) {
			diff_value = Builder->CreateLoad(diff_value);
		}
		Value *NextVar = Builder->CreateAdd(var, diff_value, "ADD");
		Value *endVal = end->accept(this);
		endVal = Builder->CreateICmpSGE(endVal, NextVar, "loopcond");
		BasicBlock *LoopEndBB = Builder->GetInsertBlock();
		Builder->CreateCondBr(endVal, LoopBB, AfterBB);
		var->addIncoming(NextVar, LoopEndBB);
	}
	Builder->SetInsertPoint(AfterBB);
	//loops.pop();

	return Builder->getInt32(0);

}
Value* Interpreter::visit(ASTWhileStatementDeclNode *node) {
	BasicBlock *loop = BasicBlock::Create(getGlobalContext(), "whileloop");
	BasicBlock *after = BasicBlock::Create(getGlobalContext(), "afterwhileloop");
	ASTBlock* while_body = node->getWhileBlock();
	Value *v3 = node->getExpression()->accept(this);
	cout << "While condition evaluated" << endl;
	if (v3->getType()->isIntegerTy(32)) {
		v3 = Builder->CreateICmpEQ(v3, Builder->getInt32(1), "cmp");
	}
	else {
		v3 = Builder->CreateICmpEQ(v3, Builder->getInt1(1), "cmp");
	}
	Value *ifcon = Builder->CreateICmpNE(v3, Builder->getInt1(0), "whilecon");
	Builder->CreateCondBr(ifcon, loop, after);
	Builder->SetInsertPoint(loop);
	cout << "while body code addded" << endl;

	Value *v = while_body->accept(this);
	cout << "while body code addded" << endl;
	v3 = node->getExpression()->accept(this);
	Value *ifcon1 = Builder->CreateICmpNE(v3, Builder->getInt1(0), "whilecon");
	Builder->CreateCondBr(ifcon1, loop, after);
	Builder->SetInsertPoint(after);
	//Ea = after;







}
Value* Interpreter::visit(ASTReadNode* node) {
	return nullptr;
}
Value* Interpreter::visit(ASTStatementDeclListNode *node) {
	return nullptr;
}

Value* Interpreter::visit(ASTBoolLiteralExpressionNode* node) {
	cout << "returning " << node->getValue() << endl;
	return Builder->getInt32(node->getValue());
}
Value* Interpreter::visit(ASTLocationExpressionNode* node) {
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




Value* ASTIntegerLiteralExpressionNode::accept( Visitor *v) {
	return v->visit(this);
}
Value* ASTLocationNode::accept( Visitor *v) {
	return v->visit(this);
}
Value* ASTAssignmentStatementNode::accept( Visitor *v) {
	return v->visit(this);
}
Value* ASTBlock::accept( Visitor *v) {
	return v->visit(this);
}
Value* ASTGotoDeclNode::accept( Visitor *v) {
	return v->visit(this);
}
Value* ASTPrintNode::accept( Visitor *v) {
	return v->visit(this);
}
Value* ASTPrintLitNode::accept( Visitor *v) {
	return v->visit(this);
}
Value* ASTBinaryExpressionNode::accept( Visitor *v) {
	return v->visit(this);
}
Value* ASTIfStatementDeclNode::accept( Visitor *v) {
	return v->visit(this);
}
Value* ASTForStatementDeclNode::accept( Visitor *v) {
	return v->visit(this);
}
Value* ASTWhileStatementDeclNode::accept( Visitor *v) {
	return v->visit(this);
}
Value* ASTReadNode::accept( Visitor *v) {
	return v->visit(this);
}
Value* ASTStatementDeclListNode::accept( Visitor *v) {
	return v->visit(this);
}

Value* ASTBoolLiteralExpressionNode::accept(Visitor *v) {
	return v->visit(this);
}
Value* ASTLocationExpressionNode::accept(Visitor *v) {
	return v->visit(this);
}

Value* ASTProgramNode::accept(Visitor *v) {
	return v->visit(this);
}
