#include "exp.h"

using namespace std;

//Expression Class

Expression::Expression()
{

}

Expression::~Expression()
{

}

int Expression::getConstantValue() {
	error("getConstantValue: Illegal expression type");
	return 0;
}

std::string Expression::getIdentifierName() {
	error("getIdentifierName: Illegal expression type");
	return "";
}

std::string Expression::getOperator() {
	error("getOperator: Illegal expression type");
	return "";
}

Expression* Expression::getLHS() {
	error("getLHS: Illegal expression type");
	return NULL;
}

Expression* Expression::getRHS() {
	error("getRHS: Illegal expression type");
	return NULL;
}

//ConstantExp Class

ConstantExp::ConstantExp(int value) {
	this->value_ = value;
}

int ConstantExp::eval(EvaluationContext& context) {
	return value_;
}

string ConstantExp::toString() {
	return integerToString(value_);
}

ExpressionType ConstantExp::getType() {
	return CONSTANT;
}

int ConstantExp::getConstantValue() {
	return value_;
}

//IdentifierExp Class

IdentifierExp::IdentifierExp(string name) {
	this->name = name;
}

int IdentifierExp::eval(EvaluationContext& context) {
	if (!context.isDefined(name))
		error(name + " is undefined");
	return context.getValue(name);
}

string IdentifierExp::toString() {
	return name;
}

ExpressionType IdentifierExp::getType() {
	return	IDENTIFIER;
}

string IdentifierExp::getIdentifierName() {
	return name;
}

//CompoundExp Class
CompoundExp::CompoundExp(string op, Expression *lhs, Expression *rhs) {
	this->op = op;
	this->lhs = lhs;
	this->rhs = rhs;
}

CompoundExp::~CompoundExp() {
	delete rhs;
	delete lhs;
}

int CompoundExp::eval(EvaluationContext& context) {
	int right = rhs->eval(context);
	if (op == "=") {
		context.setValue(lhs->getIdentifierName(), right);
		return right;
	}
	int left = lhs->eval(context);
	if (op == "+") return left + right;
	if (op == "-") return left - right;
	if (op == "*") return left * right;
	if (op == "/") {
		if (right == 0)
			error("Division by 0");
		return left / right;
	}
	error("Illegal operator in expression");
	return 0;
}

string CompoundExp::toString() {
	return '(' + lhs->toString() + ' ' + op + ' ' + rhs->toString() + ')';
}

ExpressionType CompoundExp::getType() {
	return COMPOUND;
}

string CompoundExp::getOp() {
	return op;
}

Expression* CompoundExp::getLHS() {
	return lhs;
}

Expression* CompoundExp::getRHS() {
	return rhs;
}

//EvaluationContext Class
void EvaluationContext::setValue(string var, int value) {
	symbolTable.put(var, value);
}

int EvaluationContext::getValue(string var) {
	return symbolTable.get(var);
}

bool EvaluationContext::isDefined(string var) {
	return symbolTable.contains(var);
}