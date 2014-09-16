#ifndef _tokenizer_h_
#define _tokenizer_h_

#include <string>

namespace TokenID
{
	enum Type { Identifier, Integer, String, BadToken, NumTypes };
}

class Token 
{ 
public:
	friend class Tokenizer;

	Token();
	const TokenID::Type GetType() const;
	const std::string& AsString() const;
	const int AsInt() const;

private:
	TokenID::Type m_type; 
	std::string m_string; 
};

class Tokenizer
{
public:
	Tokenizer(const std::string& str = "", const std::string& delim = "\t ");
	bool GetNextToken(Token& outToken);
	void SetString(const std::string& str, const std::string& delim = "\t ");
	void SetDelimiters(const std::string& delim);
	void ResetDelimiters();

private:
	bool IsDelimiter(const char& chr);
	bool ParseIdentifier(Token& outToken);
	bool ParseInt(Token& outToken);
	bool ParseString(Token& outToken);

	std::string m_text;
	std::string m_delimiters;
	std::string m_outStr;
	int m_index;
	int m_endPos;
};

#endif //_tokenizer_h_