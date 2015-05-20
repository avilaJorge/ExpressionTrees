#ifndef _exp_h
#define _exp_h

#include "map.h"
#include "TokenScanner.h"
#include "error.h"
#include "strlib.h"

//Code on pages 850-854

//Forward Declaration
class EvaluationContext;

enum ExpressionType { CONSTANT, IDENTIFIER, COMPOUND };

class Expression
{
public:
	Expression();
	virtual ~Expression();
	virtual int eval(EvaluationContext& context) = 0;
	virtual std::string toString() = 0;
	virtual ExpressionType getType() = 0;

	virtual int getConstantValue();
	virtual std::string getIdentifierName();
	virtual std::string getOperator();
	virtual Expression *getLHS();
	virtual Expression *getRHS();
};

class ConstantExp : public Expression {
public:
	ConstantExp(int value);

	//Virtual methods overriden by this class
	virtual int eval(EvaluationContext& context);
	virtual std::string toString();
	virtual ExpressionType getType();
	virtual int getConstantValue();
private:
	int value_;
};

class IdentifierExp : public Expression {
public:
	IdentifierExp(std::string name);

	//Virtual methods overriden by this class
	virtual int eval(EvaluationContext& context);
	virtual std::string toString();
	virtual ExpressionType getType();
	virtual std::string getIdentifierName();

private:
	std::string name;
};

class CompoundExp : public Expression {
public:
	CompoundExp(std::string op, Expression *lhs, Expression *rhs);

	//Virtual methods overriden by this class
	virtual ~CompoundExp();
	virtual int eval(EvaluationContext& context);
	virtual std::string toString();
	virtual ExpressionType getType();
	virtual std::string getOp();
	virtual Expression *getLHS();
	virtual Expression *getRHS();

private:
	std::string op;
	Expression *lhs, *rhs;	
};

class EvaluationContext {
public:
	void setValue(std::string var, int value);
	int getValue(std::string var);
	bool isDefined(std::string var);
private:
	Map<std::string, int> symbolTable;
};
#endif