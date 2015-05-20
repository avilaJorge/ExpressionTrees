#pragma once

#include "exp.h"

Expression* parseExp(TokenScanner& scanner);
Expression* readE(TokenScanner& scanner, int prec);
Expression* readT(TokenScanner& scanner);
int precedence(std::string token);
