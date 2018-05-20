#pragma once
#include <exception>
#include <string>

class LexicalError : public std::exception {
public:
	LexicalError(const std::string text) : _text(text) {
		_message = std::string("Lexical error: " + _text);
	};

	virtual const char* what() const throw() {
		return _message.c_str();
	}

private:
	const std::string _text;
	std::string _message;
};

class SyntaxError : public std::exception {
public:
	SyntaxError(const std::string text) : _text(text) {
		_message = std::string("Syntax error: " + _text);
	};

	virtual const char* what() const throw() {
		return _message.c_str();
	}
private:
	const std::string _text;
	std::string _message;
};