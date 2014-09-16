#include "tokenizer.h"
#include "core.h"
#include <iostream>
#include <fstream>

//////////////////////
// Token			//
//////////////////////

Token::Token() :
	m_type(TokenID::BadToken),
	m_string("")
{

}

const TokenID::Type Token::GetType() const
{
	return m_type;
}

const std::string& Token::AsString() const
{
	return m_string;
}

const int Token::AsInt() const
{
	assert(m_type == TokenID::Integer);
	return atoi(m_string.c_str());
}

//////////////////////
// Tokenizer		//
//////////////////////

Tokenizer::Tokenizer(const std::string& str, const std::string& delim) : 
	m_text(str),
	m_delimiters(delim),
	m_outStr(""),
	m_index(0),
	m_endPos(str.length())
{

}

bool Tokenizer::GetNextToken(Token& outToken)
{
	m_outStr = "";
	outToken.m_type = TokenID::BadToken;

	// test for empty source string
	if (m_index >= m_endPos)
	{
		outToken.m_string = m_outStr;
		return false;
	}

	// test for comment at opening index
	if (m_text[m_index] == COMMENT_CHAR)
	{
		m_endPos = m_index;
		outToken.m_string = m_outStr;
		return false;
	}

	// skip tabs and empty space at the front, bail out if we hit a comment and
	// mark the source string as dead by setting the end to be the current index
	while (IsDelimiter(m_text[m_index]))
	{
		++m_index;
		if (m_text[m_index] == COMMENT_CHAR)
			m_endPos = m_index;
		if (m_index >= m_endPos)
		{
			outToken.m_string = m_outStr;
			return false;
		}
	}

	if (isalpha(m_text[m_index]))
	{
		return ParseIdentifier(outToken);		
	}

	if ((m_text[m_index] >= '0') && (m_text[m_index] <= '9'))
	{
		return ParseInt(outToken);		
	}

	if (m_text[m_index] == '\"')
	{
		return ParseString(outToken);		
	}

	assert(false && "Tokenization error, unclassifiable token!");
	outToken.m_string = m_outStr;
	outToken.m_type = TokenID::BadToken;
	return false;
}

void Tokenizer::SetString(const std::string& str, const std::string& delim)
{
	m_text = str;
	m_delimiters = delim;
	m_index = 0;
	m_endPos = str.length();
}

void Tokenizer::SetDelimiters(const std::string& delim)
{
	m_delimiters = delim;
}

void Tokenizer::ResetDelimiters()
{
	SetDelimiters("\t ");
}

bool Tokenizer::IsDelimiter(const char& chr)
{
	int found = m_delimiters.find(chr);
	if (found != std::string::npos)
		return true;
	else
		return false;
}

bool Tokenizer::ParseIdentifier(Token& outToken)
{
	while (isalpha(m_text[m_index]))
	{
		m_outStr += m_text[m_index];
		++m_index;

		if ((m_index >= m_endPos) || (IsDelimiter(m_text[m_index])))
		{
			outToken.m_string = m_outStr;
			outToken.m_type = TokenID::Identifier;
			return true;
		}
	}

	assert(false && "Tokenization error while trying to parse an identifier, non-alpha char found!");
	outToken.m_type = TokenID::BadToken;
	return false;
}

bool Tokenizer::ParseInt(Token& outToken)
{
	while ((m_text[m_index] >= '0') && (m_text[m_index] <= '9'))
	{
		m_outStr += m_text[m_index];
		++m_index;
		
		if ((m_index >= m_endPos) || (IsDelimiter(m_text[m_index])))
		{
			outToken.m_string = m_outStr;
			outToken.m_type = TokenID::Integer;
			return true;
		}
	}

	assert(false && "Tokenization error while trying to parse an integer, non-digit found!");
	outToken.m_type = TokenID::BadToken;
	return false;
}

bool Tokenizer::ParseString(Token& outToken)
{
	SetDelimiters("\"");
	++m_index;

	while (!IsDelimiter(m_text[m_index]))
	{
		m_outStr += m_text[m_index];
		++m_index;

		// closing quote found, valid token
		if (IsDelimiter(m_text[m_index]))
		{
			outToken.m_string = m_outStr;
			outToken.m_type = TokenID::String;
			++m_index;	// move past the closing quote
			ResetDelimiters();
			return true;
		}

		// got to the end of the string and found no closing quote, invalid token
		if (m_index >= m_endPos)
		{
			assert(false && "Tokenization error while trying to parse a string, no closing quote!");

			outToken.m_string = "";
			outToken.m_type = TokenID::BadToken;
			ResetDelimiters();
			return false;
		}
	}

	assert(false && "Tokenization error while trying to parse a string!");
	return false;
}
