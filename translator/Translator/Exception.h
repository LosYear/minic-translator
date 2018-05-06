#pragma once
#include <exception>
#include <string>

class LexicalError : public std::exception {
public:
	LexicalError(const std::string text) : _text(text){};

	virtual const char* what() const throw() {
		return ("Lexical error: " + _text).c_str();
	}

private:
	const std::string _text;
};

class SyntaxError : public std::exception {
public:
	SyntaxError(const std::string text) : _text(text) {};

	virtual const char* what() const throw() {
		return ("Syntax error: " + _text).c_str();
	}
private:
	const std::string _text;
};