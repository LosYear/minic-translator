#include <iostream>
#include <string>
#pragma once

enum class LexemType {
	num, chr, str, id, lpar, rpar, lbrace, rbrace, lbracket, rbracket,
	semicolon, comma, colon, opassign, opplus, opminus, opmult, opinc, opeq, opne, oplt,
	opgt, ople, opnot, opor, opand, kwint, kwchar, kwif, kwelse, kwswitch, kwcase, kwwhile,
	kwfor, kwreturn, kwin, kwout, eof, error
};

// Class represents tokens got during lexical analysis
class LexicalToken {
public:
	LexicalToken(LexemType type);
	LexicalToken(int value);
	LexicalToken(LexemType type, const std::string &str);
	LexicalToken(char c);

	// Prints token to the stream
	void print(std::ostream &stream) const;

	// Represents token as a string
	std::string toString() const;

	// Returns type of given lexem
	LexemType type() const;

	// Returns value of given lexem
	int value() const;

	// Returns string value of given lexem
	std::string str() const;

	~LexicalToken();

private:
	// Time of given lexem
	const LexemType _type;

	// Integer value of lexem
	const int _value = 0;

	// String value of lexem
	const std::string _str = "";

	std::string lexemName() const;
};