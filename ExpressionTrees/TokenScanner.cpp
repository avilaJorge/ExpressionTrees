#include "TokenScanner.h"
#include <cctype>


TokenScanner::TokenScanner()
{
	ignoreWhitespace_ = false;
	scanStrings_ = false;
	scanNumbers_ = false;
	location_ = 0;
}

TokenScanner::TokenScanner(std::string buffer)
{
	ignoreWhitespace_ = false;
	scanStrings_ = false;
	scanNumbers_ = false;
	setInput(buffer);
}

void TokenScanner::setInput(std::string buffer)
{
	buffer_ = buffer;
	location_ = 0;
}

bool TokenScanner::hasMoreTokens()
{
	if (ignoreWhitespace_)
	{
		skipWhitespace();
	}
	return (location_ < buffer_.size());
}

std::string TokenScanner::nextToken()
{
	if (!savedTokens_.empty())
	{
		std::string token = savedTokens_.top();
		savedTokens_.pop();
		return token;
	}

	if (ignoreWhitespace_)
	{
		skipWhitespace();
	}

	if (location_ >= buffer_.size())
	{
		return "";
	}
	else if (isalpha(buffer_[location_]))
	{
		int start = location_;
		while (location_ < buffer_.size() && isalnum(buffer_[location_]))
		{
			location_++;
		}
		return buffer_.substr(start, location_ - start);
	}
	//Check the s_0 trigger of the number fsm
	else if (isdigit(buffer_[location_]))
	{
		int start = location_;
		bool done = false;
		location_++;
		while (location_ < buffer_.size() && !done)
		{
			if (buffer_[location_] == '.' || buffer_[location_] == 'e')
			{
				location_++;
				if (buffer_[location_] == '+' || buffer_[location_] == '-')
					location_++;
			}
			else if (isdigit(buffer_[location_]))
			{
				location_++;
			}
			else
			{
				done = true;
				return buffer_.substr(start, location_ - start);
			}
		}
		return buffer_.substr(start, location_ - start);
	}
	else
	{
		//Single character non-alphanumeric code
		int start = location_;
		char singleCharacter = buffer_[location_];
		if (scanStrings_ == true && (singleCharacter == '\"' || singleCharacter == '\''))
		{
			int index = location_ + 1;
			while (index < buffer_.size() && buffer_[index] != singleCharacter)
			{
				index++;
			}
			if (buffer_[index] == singleCharacter)
			{
				location_ = index + 1;
				return  buffer_.substr(start, location_ - start);
			}
		}
		location_++;
		return std::string(1, singleCharacter);
	}
}

void TokenScanner::ignoreWhitespace()
{
	ignoreWhitespace_ = true;
}

void TokenScanner::skipWhitespace()
{

	while (location_ < buffer_.size() && isspace(buffer_[location_]))
	{
		location_++;
	}
}

void TokenScanner::saveToken(std::string token)
{
	savedTokens_.push(token);
}

void TokenScanner::scanStrings()
{
	scanStrings_ = true;
}

void TokenScanner::scanNumbers()
{
	scanNumbers_ = false;
}

TokenType TokenScanner::getTokenType(std::string token) const
{
	if (token == "")
		return EMPTY;
	else
	{
		for (auto character : token)
		{
			if (!std::isdigit(character))
				return WORD;
		}
		return NUMBER;
	}
}

int TokenScanner::getTokenCount() const
{
	return buffer_.size();
}