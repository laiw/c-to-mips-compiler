#ifndef AST_H
#define	AST_H

#include <iostream>
#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <stack>




/*
IMPORTANT : consider using int counts to traverse the AST later on;
create a pretty print function that traverses through the entire tree!
]

http://stackoverflow.com/questions/10739923/what-is-in-bison
REVELATION : $$ is returned as $x !

Example:
shift_expression : additive_expression                                   { $1->test_print();}
this means that $1 is whatever I returned to $$ for additive expressions!,
in this case $$ = new Plus !


Consider using smart pointers
http://stackoverflow.com/questions/106508/what-is-a-smart-pointer-and-when-should-i-use-one

TODO:
Statements
Register tracking - look at notes
CodeGen - look at phone pic, notes

1. track all declared variables, int X ..., only 2 places where declarations like int X can occur - declaration and parameter declaration
*/

// Base node class for AST
class Node{
public:
	// virtual void postTraversal(Expression* exp1);
	virtual ~Node() {}
};


// Expressions

class Expression : public Node{
public:
	// const Type * getType();
	// virtual evaluate() = 0;
	// Render using current identifier-register bindings in ctxt
	virtual std::string getType() const;
	virtual void printer() const {}
	virtual const Expression *getLeft() const {}
	virtual const Expression *getRight() const {}
	virtual const Expression *getNext() const {}
	virtual std::string getName() const {}
	virtual int getConstant() const {}
	virtual std::string getOperator() const {}
	//virtual void evaluate() const {}
	//virtual int getSum() const {}

};


// generic class for plus,minus for 2 operands and 1 operator
class BinaryExpression : public Expression{
private:
	Expression *left = NULL;
	Expression *right = NULL;
	std::string opCode;
public:
	BinaryExpression(Expression *l,std::string op,Expression *r): left(l),opCode(op),right(r) {}
	const Expression *getLeft() const;
	std::string getOperator() const;
	const Expression *getRight() const;
	std::string getType() const;
	void printer() const;
	//void codeGen();			takes left expression and right expression, does an operation on them ( according to opCode )
//	void evaluate() const;
	//int getSum() const;
};

class UnaryExpression : public Expression{
	Expression* exp1 = NULL;
	std::string type;
public:
	UnaryExpression(Expression* exp,std::string ExpType): exp1(exp),type(ExpType) {}
	const Expression *getNext() const;
	std::string getType() const;
	void printer() const;
};

class IdentifierExpression : public Expression{
	std::string id;
public:
	std::string getName() const;
	IdentifierExpression(std::string str1): id(str1) {}
	std::string getType() const;
	void printer() const;
};

class ConstantExpression : public Expression{
	int num;
public:
	int getConstant() const;
	ConstantExpression(const int &num1) : num(num1) {}
	std::string getType() const;
	void printer() const;
};

class BracketExpression : public Expression{
	std::string leftBracket;
	std::string rightBracket;
	Expression* exp1;
public:
	BracketExpression(std::string leftBrac,std::string rightBrac,Expression* exp): leftBracket(leftBrac),rightBracket(rightBrac),exp1(exp) {}
	std::string getType() const;
	void printer() const;
};

class StackExpression : public Expression{
public:
	std::string getType() const;
};



// Statements

class Statement : public Node{
	// Render using current identifier-register bindings in ctxt
	// Context renderAssembly(const Context & ctxt) const;
	virtual void print() {}
};

class LabeledStatement : public Statement{
	
};

class CompoundStatement : public Statement{
	/*int getDeclarationCount() const;
	const Declaration *getDeclaration(int i) const;
	int getStatementCount() const;
	const Statement *getStatement(int i) const;*/
};

class ExpressionStatement : public Statement{
	const Expression *getExpression() const;
};

class SelectionStatement : public Statement{

};


/*

selection_statement : IF '(' expression ')' statement
                    | IF '(' expression ')' statement ELSE statement
                    | SWITCH '(' expression ')' statement
                    ;

*/                

class IfStatement : public Statement{
	Expression* exp;
	Statement* state;
public:
	const Expression *getCondition() const;
	const Statement *getStatement() const;
	IfStatement(Expression* exp1,Statement* state1): exp(exp1), state(state1) {}
	// IfStatement(Expression* exp1,Statement* state1,Statement* state2):
};


class IterationStatement : public Statement{
	
};

class JumpStatement : public Statement{
	
};








class AssignmentOperator : public Expression{
	std::string assignment;
public:
	AssignmentOperator(std::string value): assignment(value) {}
	std::string getType();
};

// "struct with properties associated with a register"
class Register{
public:
	Register() {}
	Register(int v,std::string var,bool t): value(v),varName(var),inUse(t) {}
	int value = 0;
	std::string varName = "";
	bool inUse = false;			// can't use 0 to check if register is in use as register can be 
};

// Register handling
// declaration - int a = 3; should use a new register
// usage - a = 5; should use an existing register regardless of scope as tested in C90 ( except in the case of function calls)
class mipsRegisters{
	std::vector<Register> registers;	// sets 31 registers to 0,make this public?
public:
	mipsRegisters():registers(32) {}
	Register getValue(const int &registerName);
	void Bind(const int &val,const int &registerName,const std::string &var);
	int registerLookup(const std::string &varName);
	void clearRegisters();
	void printAllRegisters();
	int findEmptyRegister();
};



class mips_stack{
	std::map<std::string,std::string> varToRegister;
	int stack_counter = 0;
	int param_counter = 4;
public:
	void Insert(std::string var);
	std::string get_stack_counter();
	std::string getStackOffset(std::string identifier);
	void InsertParams(std::string str1);
	void noDeclare_singleHandler(std::vector<Expression*> &completeTree,std::string declarator);
	void singleHandler(std::vector<Expression*> &completeTree,std::string declarator);
	void returnHandler(std::vector<Expression*> &completeTree);
	void ShuntingYardAlgo(std::vector<Expression*> &completeTree,std::stack<Expression*> &mystack,const bool &debugMode,std::string declarator,std::string assignOp);
};

void mips_stack::InsertParams(std::string str1){
	// allows a maximum of 4 params - TODO, "sneakily use other registers to store params"
	Insert(str1);
	std::stringstream ss;
	ss << "$" << param_counter;
	std::cout << "sw 			" << ss.str() << "," << getStackOffset(str1) << std::endl;
	param_counter++;
}


void mips_stack::singleHandler(std::vector<Expression*> &completeTree,std::string declarator){
	int identifiers = 0;
	int constants = 0;

	for(int i=0;i<completeTree.size();i++){
		if(completeTree[i]->getType() == "Constant"){
			constants++;
		}
		else if(completeTree[i]->getType() == "Identifier"){
			identifiers++;
		}
	}
	if(constants == 1 && identifiers == 0){
		// handle single constant
		int ans;
		for(int i=0;i<completeTree.size();i++){
			if(completeTree[i]->getType() == "Constant"){
				ans = completeTree[i]->getConstant();
			}
		}
		std::stringstream ss;
		ss << ans;
		Insert(declarator);
		std::string offset = getStackOffset(declarator);
		std::cout << "lw 		$2," << offset << std::endl;
		std::cout << "li 		$2," << ss.str() << std::endl;
		std::cout << "sw 		$2," << offset << std::endl;
	}
	else if(constants == 0 && identifiers == 1){
		// handle single identifier
		std::string ans;
		for(int i=0;i<completeTree.size();i++){
			if(completeTree[i]->getType() == "Identifier"){
				ans = getStackOffset(completeTree[i]->getName());
			}
		}
		Insert(declarator);
		std::string offset = getStackOffset(declarator);
		std::string RHS_offset = getStackOffset(ans);
		std::cout << "lw 		$2," << RHS_offset << std::endl;
		std::cout << "sw 		$2," << offset << std::endl;
	}
}

void mips_stack::noDeclare_singleHandler(std::vector<Expression*> &completeTree,std::string declarator){
	int identifiers = 0;
	int constants = 0;

	for(int i=0;i<completeTree.size();i++){
		if(completeTree[i]->getType() == "Constant"){
			constants++;
		}
		else if(completeTree[i]->getType() == "Identifier"){
			identifiers++;
		}
	}
	if(constants == 1 && identifiers == 1){
		// handle a = 3
		int ans;
		for(int i=0;i<completeTree.size();i++){
			if(completeTree[i]->getType() == "Constant"){
				ans = completeTree[i]->getConstant();
			}
		}
		std::stringstream ss;
		ss << ans;
		std::string offset = getStackOffset(declarator);
		std::cout << "lw 		$2," << offset << std::endl;
		std::cout << "li 		$2," << ss.str() << std::endl;
		std::cout << "sw 		$2," << offset << std::endl;
	}
	else if(constants == 0 && identifiers == 2){
		// handle b = a
		std::vector<std::string> ans;
		for(int i=0;i<completeTree.size();i++){
			if(completeTree[i]->getType() == "Identifier"){
				ans.push_back(getStackOffset(completeTree[i]->getName()));
			}
		}
		std::string offset = getStackOffset(declarator);
		std::string RHS_offset = getStackOffset(ans[1]);
		std::cout << "lw 		$2," << RHS_offset << std::endl;
		std::cout << "sw 		$2," << offset << std::endl;
	}
}

/*
a   = $4


map[$4] = "8($sp)";

int x = a;

li $2, 5
sw $2,24($sp);



int x = 1 + 2;

ALSO,
for long int arithmetic

int x = 5 + 2 + 3 - a + b - 2 * cc + d;

use shunting yard algo,
step by step,
5 + 2 = addiu $2,$2,


*/


#endif
