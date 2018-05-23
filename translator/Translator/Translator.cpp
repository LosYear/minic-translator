#include "Translator.h"
#include "Exception.h"
#include <iomanip>

Translator::Translator(std::istream & stream, std::ostream& errStream) : _lexicalAnalyzer(LexicalScanner(stream)), _currentLexem(nullptr),
_currentLabelId(0), _errStream(errStream), _lexemHistory(LexemHistory(4)) {
	_getNextLexem();
}

void Translator::printAtoms(std::ostream & stream, const unsigned int width) const
{
	for (auto context = _atoms.begin(); context != _atoms.end(); ++context) {
		for (unsigned int i = 0; i < context->second.size(); ++i) {
			stream << std::setw(width) << context->first;
			stream << " " << (context->second[i])->toString();

			if (!(std::next(context) == _atoms.end() && i == context->second.size() - 1)) {
				stream << std::endl;
			}
		}
	}
}

void Translator::printSymbolTable(std::ostream & stream) const
{
	stream << _symbolTable;
}

void Translator::generateAtom(std::unique_ptr<Atom> atom, Scope scope)
{
	_atoms[scope].push_back(std::move(atom));
}

std::shared_ptr<LabelOperand> Translator::newLabel()
{
	return std::make_shared<LabelOperand>(_currentLabelId++);
}

void Translator::throwSyntaxError(const std::string & text) const
{
	std::string errorMessage = text + "\nAfter lexems:";
	auto lexems = _lexemHistory.getAll();

	for (unsigned int i = 0; i < lexems.size() - 1; ++i) {
		errorMessage += " " + lexems[i].toString();
	}

	throw SyntaxError(errorMessage);
}

void Translator::throwLexicalError(const std::string & text) const
{
	std::string errorMessage = text + "\nAfter lexems:";
	auto lexems = _lexemHistory.getAll();

	for (unsigned int i = 0; i < lexems.size() - 1; ++i) {
		errorMessage += " " + lexems[i].toString();
	}

	throw LexicalError(errorMessage);
}

bool Translator::translate()
{
	try {
		StmtList(SymbolTable::GLOBAL_SCOPE);

		// Are any untaken lexems?
		if (_currentLexem->type() != LexemType::eof) {
			throwSyntaxError("Excess lexems are left after translation");
		}

		return true;
	}
	catch (const LexicalError& error) {
		_errStream << error.what();
		return false;
	}
	catch (const SyntaxError&  error) {
		_errStream << error.what();
		return false;
	}
}

LexicalToken Translator::_getNextLexem()
{
	_currentLexem = std::make_unique<LexicalToken>(_lexicalAnalyzer.getNextToken());
	_lexemHistory.push(*_currentLexem);

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

std::shared_ptr<RValue> Translator::translateExpresssion()
{
	return E7(SymbolTable::GLOBAL_SCOPE);
}

bool Translator::translateExpression(int)
{
	try {
		if (!E(SymbolTable::GLOBAL_SCOPE)) {
			return false;
		}

		// Are any untaken lexems?
		if (_currentLexem->type() != LexemType::eof) {
			throwSyntaxError("Excess lexems are left after translation");
		}

		return true;
	}
	catch (const LexicalError& error) {
		_errStream << error.what();
		return false;
	}
	catch (const SyntaxError&  error) {
		_errStream << error.what();
		return false;
	}
	return false;
}

std::shared_ptr<RValue> Translator::E1(const Scope context)
{
	if (_currentLexem->type() == LexemType::lpar) {
		_getNextLexem();

		std::shared_ptr<RValue> q = E(context);

		if (!q) {
			throwSyntaxError("God knows when it breakes");
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

		std::shared_ptr<MemoryOperand> q = _symbolTable.insertVar(_currentLexem->str(), context, SymbolTable::TableRecord::RecordType::unknown); // @TODO: replace with checkVar

		generateAtom(std::make_unique<BinaryOpAtom>("ADD", q, std::make_shared<NumberOperand>(1), q), context);

		_getNextLexem();

		return q;
	}
	else if (_currentLexem->type() == LexemType::id) {
		const std::string name = _currentLexem->str();
		_getNextLexem();

		// @TODO: can it break?
		return E1_(context, name);

	}

	throwSyntaxError("Rule #24-28. Unxepected lexem '" + _currentLexem->toString() + "' in expression, expected ++, (, num, id.");
	return nullptr;
}

std::shared_ptr<MemoryOperand> Translator::E1_(const Scope context, const std::string& p)
{
	if (_currentLexem->type() == LexemType::lpar) {
		throwSyntaxError("Unimplemented feature, E1_::lpar rule#30");
	}
	else if (_currentLexem->type() == LexemType::opinc) {
		_getNextLexem();

		std::shared_ptr<MemoryOperand> s = _symbolTable.insertVar(p, context, SymbolTable::TableRecord::RecordType::unknown); // @Todo:: replace with checkVar
		std::shared_ptr<MemoryOperand> r = _symbolTable.alloc(context);

		generateAtom(std::make_unique<UnaryOpAtom>("MOV", s, r), context);
		generateAtom(std::make_unique<BinaryOpAtom>("ADD", s, std::make_shared<NumberOperand>(1), s), context);

		return r;
	}

	return _symbolTable.insertVar(p, context, SymbolTable::TableRecord::RecordType::unknown); // @TODO: replace with checkVar
}

std::shared_ptr<RValue> Translator::E2(const Scope context)
{
	if (_currentLexem->type() == LexemType::opnot) {
		_getNextLexem();

		std::shared_ptr<RValue> q = E1(context);
		std::shared_ptr<MemoryOperand> r = _symbolTable.alloc(context);

		if (!q) {
			return nullptr;
		}

		generateAtom(std::make_unique<UnaryOpAtom>("NOT", q, r), context);

		return r;
	}

	return E1(context);
}

std::shared_ptr<RValue> Translator::E3(const Scope context)
{
	std::shared_ptr<RValue> q = E2(context);

	if (!q) {
		return nullptr;
	}

	std::shared_ptr<RValue> s = E3_(context, q);

	if (!s) {
		return nullptr;
	}

	return s;
}

std::shared_ptr<RValue> Translator::E3_(const Scope context, std::shared_ptr<RValue> p)
{
	if (_currentLexem->type() == LexemType::opmult) {
		_getNextLexem();

		std::shared_ptr<RValue> r = E2(context);

		if (!r) {
			return nullptr;
		}

		std::shared_ptr<MemoryOperand> s = _symbolTable.alloc(context);

		generateAtom(std::make_unique<BinaryOpAtom>("MUL", p, r, s), context);

		std::shared_ptr<RValue> t = E3_(context, s);

		if (!t) {
			return nullptr;
		}

		return t;
	}

	return p;
}

std::shared_ptr<RValue> Translator::E4(const Scope context)
{
	std::shared_ptr<RValue> q = E3(context);

	if (!q) {
		return nullptr;
	}

	std::shared_ptr<RValue> s = E4_(context, q);

	if (!s) {
		return nullptr;
	}

	return s;
}

std::shared_ptr<RValue> Translator::E4_(const Scope context, std::shared_ptr<RValue> p)
{
	if (_currentLexem->type() == LexemType::opplus) {
		_getNextLexem();

		std::shared_ptr<RValue> r = E3(context);

		if (!r) {
			return nullptr;
		}

		std::shared_ptr<MemoryOperand> s = _symbolTable.alloc(context);

		generateAtom(std::make_unique<BinaryOpAtom>("ADD", p, r, s), context);

		std::shared_ptr<RValue> t = E4_(context, s);

		if (!t) {
			return nullptr;
		}

		return t;
	}
	else if (_currentLexem->type() == LexemType::opminus) {
		_getNextLexem();

		std::shared_ptr<RValue> r = E3(context);

		if (!r) {
			return nullptr;
		}

		std::shared_ptr<MemoryOperand> s = _symbolTable.alloc(context);

		generateAtom(std::make_unique<BinaryOpAtom>("SUB", p, r, s), context);

		std::shared_ptr<RValue> t = E4_(context, s);

		if (!t) {
			return nullptr;
		}

		return t;
	}

	return p;
}

std::shared_ptr<RValue> Translator::E5(const Scope context)
{
	std::shared_ptr<RValue> q = E4(context);

	if (!q) {
		return nullptr;
	}

	std::shared_ptr<RValue> s = E5_(context, q);

	if (!s) {
		return nullptr;
	}

	return s;
}

std::shared_ptr<RValue> Translator::E5_(const Scope context, std::shared_ptr<RValue> p)
{
	if (_currentLexem->type() == LexemType::opeq || _currentLexem->type() == LexemType::opne ||
		_currentLexem->type() == LexemType::opgt || _currentLexem->type() == LexemType::oplt ||
		_currentLexem->type() == LexemType::ople) {
		LexemType currentLexem = _currentLexem->type();
		_getNextLexem();

		std::shared_ptr<RValue> r = E4(context);

		if (!r) {
			return nullptr;
		}

		std::shared_ptr<MemoryOperand> s = _symbolTable.alloc(context);
		std::shared_ptr<LabelOperand> l = newLabel();

		generateAtom(std::make_unique<UnaryOpAtom>("MOV", std::make_shared<NumberOperand>(1), s), context);

		if (currentLexem == LexemType::opeq) {
			generateAtom(std::make_unique<ConditionalJumpAtom>("EQ", p, r, l), context);
		}
		else if (currentLexem == LexemType::opne) {
			generateAtom(std::make_unique<ConditionalJumpAtom>("NE", p, r, l), context);
		}
		else if (currentLexem == LexemType::opgt) {
			generateAtom(std::make_unique<ConditionalJumpAtom>("GT", p, r, l), context);
		}
		else if (currentLexem == LexemType::oplt) {
			generateAtom(std::make_unique<ConditionalJumpAtom>("LT", p, r, l), context);
		}
		else if (currentLexem == LexemType::ople) {
			generateAtom(std::make_unique<ConditionalJumpAtom>("LE", p, r, l), context);
		}

		generateAtom(std::make_unique<UnaryOpAtom>("MOV", std::make_shared<NumberOperand>(0), s), context);
		generateAtom(std::make_unique<LabelAtom>(l), context);

		return s;
	}

	return p;
}


std::shared_ptr<RValue> Translator::E6(const Scope context)
{
	std::shared_ptr<RValue> q = E5(context);

	if (!q) {
		return nullptr;
	}

	std::shared_ptr<RValue> s = E6_(context, q);

	if (!s) {
		return nullptr;
	}

	return s;
}

std::shared_ptr<RValue> Translator::E6_(const Scope context, std::shared_ptr<RValue> p)
{
	if (_currentLexem->type() == LexemType::opand) {
		_getNextLexem();

		std::shared_ptr<RValue> r = E5(context);

		if (!r) {
			return nullptr;
		}

		std::shared_ptr<MemoryOperand> s = _symbolTable.alloc(context);

		generateAtom(std::make_unique<BinaryOpAtom>("AND", p, r, s), context);

		std::shared_ptr<RValue> t = E6_(context, s);

		if (!t) {
			return nullptr;
		}

		return t;
	}

	return p;
}

std::shared_ptr<RValue> Translator::E7(const Scope context)
{
	std::shared_ptr<RValue> q = E6(context);

	if (!q) {
		return nullptr;
	}

	std::shared_ptr<RValue> s = E7_(context, q);

	if (!s) {
		return nullptr;
	}

	return s;
}

std::shared_ptr<RValue> Translator::E7_(const Scope context, std::shared_ptr<RValue> p)
{
	if (_currentLexem->type() == LexemType::opor) {
		_getNextLexem();

		std::shared_ptr<RValue> r = E6(context);

		if (!r) {
			return nullptr;
		}

		std::shared_ptr<MemoryOperand> s = _symbolTable.alloc(context);

		generateAtom(std::make_unique<BinaryOpAtom>("OR", p, r, s), context);

		std::shared_ptr<RValue> t = E7_(context, s);

		if (!t) {
			return nullptr;
		}

		return t;
	}

	return p;
}

std::shared_ptr<RValue> Translator::E(const Scope context)
{
	return E7(context);
}

void Translator::DeclareStmt(const Scope context)
{
	SymbolTable::TableRecord::RecordType p = Type();

	if (p == SymbolTable::TableRecord::RecordType::unknown) {
		throwSyntaxError("Unknown type definition.");
	}

	const std::string name = _currentLexem->str();
	_takeTerm(LexemType::id);

	DeclareStmt_(context, p, name);
}

void Translator::DeclareStmt_(const Scope context, SymbolTable::TableRecord::RecordType p, const std::string & q)
{
	if (_currentLexem->type() == LexemType::lpar) {
		if (context != SymbolTable::GLOBAL_SCOPE) {
			throwSyntaxError("Function can't be defined inside another function.");
		}
		_getNextLexem();

		Scope newContext = _symbolTable.insertFunc(q, p, -1)->index();

		unsigned int n = ParamList(newContext);
		_symbolTable.changeArgsCount(newContext, n);

		_takeTerm(LexemType::rpar);
		_takeTerm(LexemType::lbrace);

		StmtList(newContext);

		_takeTerm(LexemType::rbrace);

		generateAtom(std::make_unique<RetAtom>(std::make_shared<NumberOperand>(0)), context);
	}
	else if (_currentLexem->type() == LexemType::opassign) {
		_getNextLexem();

		int val = _currentLexem->value();

		_takeTerm(LexemType::num);

		_symbolTable.insertVar(q, context, p, val);

		DeclVarList_(context, p);

		_takeTerm(LexemType::semicolon);
	}
	else {
		_symbolTable.insertVar(q, context, p);
		DeclVarList_(context, p);
		_takeTerm(LexemType::semicolon);
	}
}

SymbolTable::TableRecord::RecordType Translator::Type()
{
	if (_currentLexem->type() == LexemType::kwchar) {
		_getNextLexem();
		return SymbolTable::TableRecord::RecordType::chr;
	}
	else if (_currentLexem->type() == LexemType::kwint) {
		_getNextLexem();
		return SymbolTable::TableRecord::RecordType::integer;
	}

	return SymbolTable::TableRecord::RecordType::unknown;
}

void Translator::DeclVarList_(const Scope context, SymbolTable::TableRecord::RecordType p)
{
	if (_currentLexem->type() == LexemType::comma) {
		_getNextLexem();

		const std::string name = _currentLexem->str();
		_takeTerm(LexemType::id);

		InitVar(context, p, name);
		DeclVarList_(context, p);
	}

	return;
}

void Translator::InitVar(const Scope context, SymbolTable::TableRecord::RecordType p, const std::string & q)
{
	if (_currentLexem->type() == LexemType::opassign) {
		_getNextLexem();

		int val = _currentLexem->value();

		if (_currentLexem->type() != LexemType::num && _currentLexem->type() != LexemType::chr) {
			throwSyntaxError("Int or char excepted as init value.");
		}

		_getNextLexem();

		_symbolTable.insertVar(q, context, p, val);
	}
	else {
		_symbolTable.insertVar(q, context, p);
	}
}

unsigned int Translator::ParamList(const Scope context)
{
	SymbolTable::TableRecord::RecordType q = Type();

	if (q == SymbolTable::TableRecord::RecordType::unknown) {
		return 0;
	}

	const std::string name = _currentLexem->str();
	_takeTerm(LexemType::id);

	_symbolTable.insertVar(name, context, q);

	return ParamList_(context) + 1;
}

unsigned int Translator::ParamList_(const Scope context)
{
	if (_currentLexem->type() == LexemType::comma) {
		_getNextLexem();

		SymbolTable::TableRecord::RecordType q = Type();

		if (q == SymbolTable::TableRecord::RecordType::unknown) {
			throwSyntaxError("Unknown type for variable. ");
		}

		const std::string name = _currentLexem->str();
		_takeTerm(LexemType::id);

		_symbolTable.insertVar(name, context, q);

		return ParamList_(context) + 1;
	}

	return 0;
}

void Translator::StmtList(const Scope context)
{
	LexemType type = _currentLexem->type();
	if (type == LexemType::kwchar || type == LexemType::kwint || type == LexemType::id
		|| type == LexemType::kwwhile || type == LexemType::kwfor || type == LexemType::kwif
		|| type == LexemType::kwswitch || type == LexemType::kwin || type == LexemType::kwin
		|| type == LexemType::kwout || type == LexemType::semicolon || type == LexemType::lbrace
		|| type == LexemType::kwreturn) {
		Stmt(context);
		StmtList(context);
	}
}

void Translator::Stmt(const Scope context)
{
	LexemType type = _currentLexem->type();

	if (type == LexemType::kwchar || type == LexemType::kwint) {
		DeclareStmt(context);
	}
	else if (type == LexemType::rbrace) {

	}
	else {

		throwSyntaxError("Forbidden lexem"); //@TODO: CHANGE 
	}
}
