#include "Translator.h"
#include "Exception.h"

Translator::Translator(std::istream & stream) : _lexicalAnalyzer(LexicalScanner(stream)), _currentLexem(nullptr), _currentLabelId(0) {
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
	throw LexicalError(text);
}

bool Translator::translate()
{

	return true;
}

LexicalToken Translator::_getNextLexem()
{
	_currentLexem = std::make_unique<LexicalToken>(_lexicalAnalyzer.getNextToken());

	if (_currentLexem->type() == LexemType::error) {
		throwLexicalError(_currentLexem->str());
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

std::shared_ptr<RValue> Translator::E1()
{
	if (_currentLexem->type() == LexemType::lpar) {
		throwSyntaxError("Unimplemented feature, E1::lpar rule#24");
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
