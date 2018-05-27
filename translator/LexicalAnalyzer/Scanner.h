#pragma once
#include <string>
#include <sstream>
#include <map>
#include <exception>

#include "Token.h"

// Class representing lexical scanner
class LexicalScanner {
public:
	LexicalScanner(std::istream& stream);

	// Gets next token in the stream
	LexicalToken getNextToken();

private:
	std::istream& _stream;
	
	// Current state of finite automata
	int _state = 0;
	std::string _value = "";

	const std::map<char, LexemType> _punctuation = {
		{ ',', LexemType::comma },
		{ ';', LexemType::semicolon },
		{ ':', LexemType::colon },
		{ '*', LexemType::opmult },
		{ '>', LexemType::opgt },
		{ '(', LexemType::lpar },
		{ ')', LexemType::rpar },
		{ '{', LexemType::lbrace },
		{ '}', LexemType::rbrace },
		{ '[', LexemType::lbracket },
		{ ']', LexemType::rbracket }
	};

	const std::map<std::string, LexemType> _keywords = { { "int", LexemType::kwint },
	{ "char", LexemType::kwchar },
	{ "if", LexemType::kwif },
	{ "else", LexemType::kwelse },
	{ "switch", LexemType::kwswitch },
	{ "case", LexemType::kwcase },
	{ "default", LexemType::kwdefault },
	{ "while", LexemType::kwwhile },
	{ "for", LexemType::kwfor },
	{ "return", LexemType::kwreturn },
	{ "in", LexemType::kwin },
	{ "out", LexemType::kwout } };

	bool isDigit(char c);
	bool isLetter(char c);
};