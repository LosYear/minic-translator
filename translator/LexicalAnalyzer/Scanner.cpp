#include "Scanner.h"

LexicalScanner::LexicalScanner(std::istream& stream) : 	_stream(stream) {}

LexicalToken LexicalScanner::getNextToken()
{
	_value = "";
	_state = 0;


	while (true) {
		char c = 0;

		_stream >> std::noskipws >> c;
		if (_state == 0) {
			// End of input stream?
			if (_stream.eof() || c == '\0') {
				return LexicalToken(LexemType::eof);
			}

			if (isDigit(c)) {
				_value = c;
				_state = 1;
				continue;
			}

			if (c == '\'') {
				_value = "";
				_state = 2;
				continue;
			}

			if (c == '"') {
				_value = "";
				_state = 4;
				continue;
			}

			if (isLetter(c)) {
				_value = c;
				_state = 5;
				continue;
			}

			if (c == '<') {
				_state = 8;
				continue;
			}

			if (c == '-') {
				_state = 6;
				continue;
			}

			if (c == '!') {
				_state = 7;
				continue;
			}

			if (c == '=') {
				_state = 9;
				continue;
			}

			if (c == '+') {
				_state = 10;
				continue;
			}

			if (c == '|') {
				_state = 11;
				continue;
			}

			if (c == '&') {
				_state = 12;
				continue;
			}

			if (_punctuation.count(c) > 0) {
				return _punctuation.at(c);
			}

			if (c == ' ' || c == '\t' || c == '\n') {
				continue;
			}

			return LexicalToken(LexemType::error, std::string("Unknown symbol '") + c + std::string("'"));

		}

		if (_state == 1) {
			if (_stream.eof() || !isDigit(c)) {
				_state = 0;
				_stream.putback(c);
				return LexicalToken(stoi(_value));
			}
			_value += c;
			continue;
		}

		if (_state == 2) {
			if (_stream.eof()) {
				return LexicalToken(LexemType::error, "Unclosed char constant at the end of file");
			}

			if (c == '\'') {
				return LexicalToken(LexemType::error, "Empty char constant");
			}

			_value = c;
			_state = 3;
			continue;
		}

		if (_state == 3) {
			if (_stream.eof()) {
				return LexicalToken(LexemType::error, "Unclosed char constant at the end of file");
			}

			if (c == '\'') {
				_state = 0;
				return LexicalToken((char)_value[0]);
			}
			return LexicalToken(LexemType::error, "Char constant has more than one symbol");
		}

		if (_state == 4) {
			if (_stream.eof()) {
				return LexicalToken(LexemType::error, "Unclosed string constant at the end of file");
			}
			if (c == '"') {
				_state = 0;
				return LexicalToken(LexemType::str, _value); // @TODO add string table?
			}
			_value += c;
			continue;
		}

		if (_state == 5) {
			if (!(_stream.eof()) && (isLetter(c) || isDigit(c))) {
				_value += c;
				continue;
			}

			if (!(_stream.eof())) {
				_stream.putback(c);
			}

			_state = 0;

			if (_keywords.count(_value) > 0) {
				return LexicalToken(_keywords.at(_value));
			}

			return LexicalToken(LexemType::id, _value);
		}

		if (_state == 6) {
			if (!_stream.eof() && isDigit(c)) {
				_value = "-" + _value + c;
				_state = 1;
				continue;
			}

			_stream.putback(c);
			_state = 0;

			return LexicalToken(LexemType::opminus);
		}

		if (_state == 7) {
			_state = 0;

			if (c == '=') {
				return LexicalToken(LexemType::opne);
			}

			_stream.putback(c);
			return LexicalToken(LexemType::opnot);
		}

		if (_state == 8) {
			_state = 0;

			if (c == '=') {
				return LexicalToken(LexemType::ople);
			}

			_stream.putback(c);
			return LexicalToken(LexemType::oplt);
		}

		if (_state == 9) {
			_state = 0;

			if (c == '=') {
				return LexicalToken(LexemType::opeq);
			}

			_stream.putback(c);
			return LexicalToken(LexemType::opassign);
		}

		if (_state == 10) {
			_state = 0;

			if (c == '+') {
				return LexicalToken(LexemType::opinc);
			}

			_stream.putback(c);
			return LexicalToken(LexemType::opplus);
		}

		if (_state == 11) {
			if (c == '|') {
				_state = 0;

				return LexicalToken(LexemType::opor);
			}

			return LexicalToken(LexemType::error, "Incomplete OR operator");
		}

		if (_state == 12) {
			if (c == '&') {
				_state = 0;

				return LexicalToken(LexemType::opand);
			}

			return LexicalToken(LexemType::error, "Incomplete AND operator");
		}

		throw std::runtime_error((std::string("Undefined state:") + std::to_string(_state).c_str()).c_str());

		//return Token(LexemType::error, "Undefined state"); // @TODO
	}
}

bool LexicalScanner::isDigit(char c)
{
	return '0' <= c && c <= '9';
}

bool LexicalScanner::isLetter(char c)
{
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}
