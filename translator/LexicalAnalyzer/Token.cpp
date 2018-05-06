#include "Token.h"

LexicalToken::LexicalToken(LexemType type) : _type(type) {
	bool allowedType = type != LexemType::num && type != LexemType::chr
		&& type != LexemType::str && type != LexemType::id && type != LexemType::error;
	if (!allowedType) {
		throw std::invalid_argument("This type of lexem requires extra param to be created");
	}
}

LexicalToken::LexicalToken(int value) : _value(value), _type(LexemType::num) {}

LexicalToken::LexicalToken(LexemType type, const std::string & str) : _str(str), _type(type)
{
	bool allowedType = type == LexemType::error || type == LexemType::str
		|| type == LexemType::id;
	if (!allowedType) {
		throw std::invalid_argument("This type of lexem can't be initialised with string value");
	}
}

LexicalToken::LexicalToken(char c) : _value(c), _type(LexemType::chr) {}

void LexicalToken::print(std::ostream& stream) const
{
	stream << toString();
}

std::string LexicalToken::toString() const
{
	if (_type == LexemType::num) {
		return "[num, " + std::to_string(_value) + "]";
	}
	else if (_type == LexemType::str) {
		return "[str, \"" + _str + "\"]";
	}
	else if (_type == LexemType::id) {
		return "[id, \"" + _str + "\"]";
	}
	else if (_type == LexemType::error) {
		return "[error, \"" + _str + "\"]";
	}
	else if (_type == LexemType::chr) {
		return std::string("[chr, '") + static_cast<char>(_value) + std::string("']");
	}
	else {
		return "[" + lexemName() + "]";
	}
}

LexemType LexicalToken::type() const
{
	return _type;
}

int LexicalToken::value() const
{
	return _value;
}

std::string LexicalToken::str() const
{
	return _str;
}

LexicalToken::~LexicalToken()
{
}

std::string LexicalToken::lexemName() const
{
	switch (_type) {
	case LexemType::num:
		return "num";
		break;
	case LexemType::chr:
		return "chr";
		break;
	case LexemType::str:
		return "str";
		break;
	case LexemType::id:
		return "id";
		break;
	case LexemType::lpar:
		return "lpar";
		break;
	case LexemType::rpar:
		return "rpar";
		break;
	case LexemType::lbrace:
		return "lbrace";
		break;
	case LexemType::rbrace:
		return "rbrace";
		break;
	case LexemType::lbracket:
		return "lbracket";
		break;
	case LexemType::rbracket:
		return "rbracket";
		break;
	case LexemType::semicolon:
		return "semicolon";
		break;
	case LexemType::comma:
		return "comma";
		break;
	case LexemType::colon:
		return "colon";
		break;
	case LexemType::opassign:
		return "opassign";
		break;
	case LexemType::opplus:
		return "opplus";
		break;
	case LexemType::opminus:
		return "opminus";
		break;
	case LexemType::opmult:
		return "opmult";
		break;
	case LexemType::opinc:
		return "opinc";
		break;
	case LexemType::opeq:
		return "opeq";
		break;
	case LexemType::opne:
		return "opne";
		break;
	case LexemType::oplt:
		return "oplt";
		break;
	case LexemType::opgt:
		return "opgt";
		break;
	case LexemType::ople:
		return "ople";
		break;
	case LexemType::opnot:
		return "opnot";
		break;
	case LexemType::opor:
		return "opor";
		break;
	case LexemType::opand:
		return "opand";
		break;
	case LexemType::kwint:
		return "kwint";
		break;
	case LexemType::kwchar:
		return "kwchar";
		break;
	case LexemType::kwif:
		return "kwif";
		break;
	case LexemType::kwelse:
		return "kwelse";
		break;
	case LexemType::kwswitch:
		return "kwswitch";
		break;
	case LexemType::kwcase:
		return "kwcase";
		break;
	case LexemType::kwwhile:
		return "kwwhile";
		break;
	case LexemType::kwfor:
		return "kwfor";
		break;
	case LexemType::kwreturn:
		return "kwreturn";
		break;
	case LexemType::kwin:
		return "kwin";
		break;
	case LexemType::kwout:
		return "kwout";
		break;
	case LexemType::eof:
		return "eof";
		break;
	case LexemType::error:
		return "error";
		break;
	default:
		return "unknown";
		break;
	}
}
