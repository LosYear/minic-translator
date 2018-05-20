#include "Translator.h"
#include "Exception.h"

Translator::Translator(std::istream & stream, std::ostream& errStream) : _lexicalAnalyzer(LexicalScanner(stream)), _currentLexem(nullptr), 
_currentLabelId(0), _errStream(errStream) {
	_getNextLexem();
}

void Translator::printAtoms(std::ostream & stream) const
{
	for (unsigned int i = 0; i < _atoms.size(); ++i) {
		stream << (_atoms[i])->toString();

		if (i != _atoms.size() - 1) {
			stream << std::endl;
		}
	}
}

void Translator::generateAtom(std::unique_ptr<Atom> atom)
{
	_atoms.push_back(std::move(atom));
}

std::shared_ptr<LabelOperand> Translator::newLabel()
{
	return std::make_shared<LabelOperand>(_currentLabelId++);
}

void Translator::throwSyntaxError(const std::string & text) const
{
	throw SyntaxError(text);
}

void Translator::throwLexicalError(const std::string & text) const
{
	std::string errorMessage = text + "\n After lexems:";
	auto lexems = _lexemHistory.getAll();

	for (unsigned int i = 0; i < lexems.size(); ++i) {
		errorMessage += " " + lexems[i].toString();
	}

	throw LexicalError(errorMessage);
}

bool Translator::translate()
{
	try {
		if (!E()) {
			return false;
		}

		return true;
	}
	catch (const LexicalError& error) {
		_errStream << error.what();
		return false;
	}
	catch (const SyntaxError&  error) {

	}
}

LexicalToken Translator::_getNextLexem()
{
	_currentLexem = std::make_unique<LexicalToken>(_lexicalAnalyzer.getNextToken());

	if (_currentLexem->type() == LexemType::error) {
		throwLexicalError(_currentLexem->str());
	}
	else {
		_lexemHistory.push(*_currentLexem);
	}

	return *_currentLexem;
}

bool Translator::_takeTerm(LexemType type)
{
	if (_currentLexem->type() != type) {
		throwSyntaxError("Excepted " + LexicalToken::lexemName(type) + ", got " + _currentLexem->toString());
		return false;
	}
	_getNextLexem();
	return true;
}

std::shared_ptr<RValue> Translator::translateExpresssion()
{
	return E7();
}

std::shared_ptr<RValue> Translator::E1()
{
	if (_currentLexem->type() == LexemType::lpar) {
		_getNextLexem();

		std::shared_ptr<RValue> q = E();

		if (!q) {
			return nullptr; // @todo: syntax error
		}

		_takeTerm(LexemType::rpar);

		return q;
	}
	else if (_currentLexem->type() == LexemType::num || _currentLexem->type() == LexemType::chr) {
		auto operand = std::make_shared<NumberOperand>(_currentLexem->value());
		_getNextLexem();

		return operand;
	}
	else if (_currentLexem->type() == LexemType::opinc) {
		_getNextLexem();

		std::shared_ptr<MemoryOperand> q = _symbolTable.insert(_currentLexem->str());

		generateAtom(std::make_unique<BinaryOpAtom>("ADD", q, std::make_shared<NumberOperand>(1), q));

		_getNextLexem();

		return q;
	}
	else if (_currentLexem->type() == LexemType::id) {
		const std::string name = _currentLexem->str();
		_getNextLexem();

		return E1_(name);

	}

	throwSyntaxError("Unxepected lexem '" + _currentLexem->toString() + "' in expression, , expected ++, (, num, id");
	return nullptr;
}

std::shared_ptr<MemoryOperand> Translator::E1_(const std::string& p)
{
	if (_currentLexem->type() == LexemType::lpar) {
		throwSyntaxError("Unimplemented feature, E1_::lpar rule#30");
	}
	else if (_currentLexem->type() == LexemType::opinc) {
		_getNextLexem();

		std::shared_ptr<MemoryOperand> s = _symbolTable.insert(p);
		std::shared_ptr<MemoryOperand> r = _symbolTable.alloc();

		generateAtom(std::make_unique<UnaryOpAtom>("MOV", s, r));
		generateAtom(std::make_unique<BinaryOpAtom>("ADD", s, std::make_shared<NumberOperand>(1), s));

		return r;
	}

	return _symbolTable.insert(p);
}

std::shared_ptr<RValue> Translator::E2()
{
	if (_currentLexem->type() == LexemType::opnot) {
		_getNextLexem();

		std::shared_ptr<RValue> q = E1();
		std::shared_ptr<MemoryOperand> r = _symbolTable.alloc();

		if (!q) {
			// @TODO :: ERROR
		}

		generateAtom(std::make_unique<UnaryOpAtom>("NOT", q, r));

		return r;
	}

	return E1();
}

std::shared_ptr<RValue> Translator::E3()
{
	std::shared_ptr<RValue> q = E2();

	if (!q) {
		return nullptr; // @TODO: syntax error
	}

	std::shared_ptr<RValue> s = E3_(q);

	if (!s) {
		return nullptr; // @TODO: syntax error
	}

	return s;
}

std::shared_ptr<RValue> Translator::E3_(std::shared_ptr<RValue> p)
{
	if (_currentLexem->type() == LexemType::opmult) {
		_getNextLexem();

		std::shared_ptr<RValue> r = E2();

		if (!r) {
			return nullptr; // @todo: syntax error
		}

		std::shared_ptr<MemoryOperand> s = _symbolTable.alloc();

		generateAtom(std::make_unique<BinaryOpAtom>("MUL", p, r, s));

		std::shared_ptr<RValue> t = E3_(s);

		if (!t) {
			return nullptr; // @Todo: syntax error
		}

		return t;
	}

	return p;
}

std::shared_ptr<RValue> Translator::E4()
{
	std::shared_ptr<RValue> q = E3();

	if (!q) {
		return nullptr; // @TODO: syntax error
	}

	std::shared_ptr<RValue> s = E4_(q);

	if (!s) {
		return nullptr; // @TODO: syntax error
	}

	return s;
}

std::shared_ptr<RValue> Translator::E4_(std::shared_ptr<RValue> p)
{
	if (_currentLexem->type() == LexemType::opplus) {
		_getNextLexem();

		std::shared_ptr<RValue> r = E3();

		if (!r) {
			return nullptr; // @todo: syntax error
		}

		std::shared_ptr<MemoryOperand> s = _symbolTable.alloc();

		generateAtom(std::make_unique<BinaryOpAtom>("ADD", p, r, s));

		std::shared_ptr<RValue> t = E4_(s);

		if (!t) {
			return nullptr; // @Todo: syntax error
		}

		return t;
	}
	else if (_currentLexem->type() == LexemType::opminus) {
		_getNextLexem();

		std::shared_ptr<RValue> r = E3();

		if (!r) {
			return nullptr; // @todo: syntax error
		}

		std::shared_ptr<MemoryOperand> s = _symbolTable.alloc();

		generateAtom(std::make_unique<BinaryOpAtom>("SUB", p, r, s));

		std::shared_ptr<RValue> t = E4_(s);

		if (!t) {
			return nullptr; // @Todo: syntax error
		}

		return t;
	}

	return p;
}

std::shared_ptr<RValue> Translator::E5()
{
	std::shared_ptr<RValue> q = E4();

	if (!q) {
		return nullptr; // @TODO: syntax error
	}

	std::shared_ptr<RValue> s = E5_(q);

	if (!s) {
		return nullptr; // @TODO: syntax error
	}

	return s;
}

std::shared_ptr<RValue> Translator::E5_(std::shared_ptr<RValue> p)
{
	if (_currentLexem->type() == LexemType::opeq || _currentLexem->type() == LexemType::opne ||
		_currentLexem->type() == LexemType::opgt || _currentLexem->type() == LexemType::oplt ||
		_currentLexem->type() == LexemType::ople) {
		LexemType currentLexem = _currentLexem->type();
		_getNextLexem();

		std::shared_ptr<RValue> r = E4();

		if (!r) {
			return nullptr; // @todo: syntax error
		}

		std::shared_ptr<MemoryOperand> s = _symbolTable.alloc();
		std::shared_ptr<LabelOperand> l = newLabel();

		generateAtom(std::make_unique<UnaryOpAtom>("MOV", std::make_shared<NumberOperand>(1), s));

		if (currentLexem == LexemType::opeq) {
			generateAtom(std::make_unique<ConditionalJumpAtom>("EQ", p, r, l));
		}
		else if (currentLexem == LexemType::opne) {
			generateAtom(std::make_unique<ConditionalJumpAtom>("NE", p, r, l));
		}
		else if (currentLexem == LexemType::opgt) {
			generateAtom(std::make_unique<ConditionalJumpAtom>("GT", p, r, l));
		}
		else if (currentLexem == LexemType::oplt) {
			generateAtom(std::make_unique<ConditionalJumpAtom>("LT", p, r, l));
		}
		else if (currentLexem == LexemType::ople) {
			generateAtom(std::make_unique<ConditionalJumpAtom>("LE", p, r, l));
		}

		generateAtom(std::make_unique<UnaryOpAtom>("MOV", std::make_shared<NumberOperand>(0), s));
		generateAtom(std::make_unique<LabelAtom>(l));

		return s;
	}

	return p;
}


std::shared_ptr<RValue> Translator::E6()
{
	std::shared_ptr<RValue> q = E5();

	if (!q) {
		return nullptr; // @TODO: syntax error
	}

	std::shared_ptr<RValue> s = E6_(q);

	if (!s) {
		return nullptr; // @TODO: syntax error
	}

	return s;
}

std::shared_ptr<RValue> Translator::E6_(std::shared_ptr<RValue> p)
{
	if (_currentLexem->type() == LexemType::opand) {
		_getNextLexem();

		std::shared_ptr<RValue> r = E5();

		if (!r) {
			return nullptr; // @todo: syntax error
		}

		std::shared_ptr<MemoryOperand> s = _symbolTable.alloc();

		generateAtom(std::make_unique<BinaryOpAtom>("AND", p, r, s));

		std::shared_ptr<RValue> t = E6_(s);

		if (!t) {
			return nullptr; //@todo : syntax error
		}

		return t;
	}

	return p;
}

std::shared_ptr<RValue> Translator::E7()
{
	std::shared_ptr<RValue> q = E6();

	if (!q) {
		return nullptr; // @TODO: syntax error
	}

	std::shared_ptr<RValue> s = E7_(q);

	if (!s) {
		return nullptr; // @TODO: syntax error
	}

	return s;
}

std::shared_ptr<RValue> Translator::E7_(std::shared_ptr<RValue> p)
{
	if (_currentLexem->type() == LexemType::opor) {
		_getNextLexem();

		std::shared_ptr<RValue> r = E6();

		if (!r) {
			return nullptr; // @todo: syntax error
		}

		std::shared_ptr<MemoryOperand> s = _symbolTable.alloc();

		generateAtom(std::make_unique<BinaryOpAtom>("OR", p, r, s));

		std::shared_ptr<RValue> t = E7_(s);

		if (!t) {
			return nullptr; //@todo : syntax error
		}

		return t;
	}

	return p;
}

std::shared_ptr<RValue> Translator::E()
{
	return E7();
}
