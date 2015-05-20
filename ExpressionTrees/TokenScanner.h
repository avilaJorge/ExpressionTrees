#pragma once
#include <string>
#include <stack>

enum TokenType {
	WORD, 
	NUMBER,
	EMPTY
};

class TokenScanner
{
public:
	TokenScanner();
	TokenScanner(std::string buffer);
	void setInput(std::string buffer);
	bool hasMoreTokens();
	std::string nextToken();
	void ignoreWhitespace();
	void saveToken(std::string token);
	void scanStrings();
	void scanNumbers();
	TokenType getTokenType(std::string token) const;
	int getTokenCount() const;
private:
	std::string buffer_;
	int location_;
	bool ignoreWhitespace_;
	bool scanStrings_;
	bool scanNumbers_;
	void skipWhitespace();
	std::stack<std::string> savedTokens_;
};