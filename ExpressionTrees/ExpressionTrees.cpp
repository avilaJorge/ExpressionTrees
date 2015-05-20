// ExpressionTrees.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TokenScanner.h"
#include <iostream>	
#include <string>
#include "parser.h"

using namespace std;

int main()
{
	bool quit = false;
	EvaluationContext context;
	TokenScanner scanner;
	Expression *exp;
	while (!quit)
	{
		exp = NULL;
		try
		{
			string line;
			cout << "=> ";
			getline(cin, line);
			if (line == "quit")
				quit = true;
			else
			{
				scanner.setInput(line);
				scanner.ignoreWhitespace();
				scanner.scanNumbers();
				Expression *exp = parseExp(scanner);
				int value = exp->eval(context);
				cout << value << endl;
			}
		}
		catch (ErrorException& ex) {
			cerr << "Error: " << ex.getMessage() << endl;
		}
		if (exp != NULL)
			delete exp;
	}

	return 0;
}

