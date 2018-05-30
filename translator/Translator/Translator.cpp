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

void Translator::printStringTable(std::ostream & stream) const
{
	stream << _stringTable;
}

void Translator::generateAtom(std::unique_ptr<Atom> atom, Scope scope)
{
	_atoms[scope].push_back(std::move(atom));
}

std::shared_ptr<MemoryOperand> Translator::insertSymbolTableVar(const std::string & name, const Scope scope, const SymbolTable::TableRecord::RecordType type, const unsigned int init)
{
	return _symbolTable.insertVar(name, scope, type, init);
}

std::shared_ptr<MemoryOperand> Translator::insertSymbolTableFunc(const std::string & name, const SymbolTable::TableRecord::RecordType type, const int len)
{
	return _symbolTable.insertFunc(name, type, len);
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

		std::shared_ptr<MemoryOperand> m = _symbolTable.checkFunc("main", 0);
		if (!m) {
			throwSyntaxError("No entry point for given program");
		}

		_symbolTable.calculateOffset();

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

void Translator::generateCode(std::ostream & stream) const
{
	stream << "ORG 8000H" << std::endl;
	_symbolTable.generateGlobalsSection(stream);
	_stringTable.generateGlobalsSection(stream);
	_generateProlog(stream);

	std::vector<unsigned int> fns = _symbolTable.functionsIds();

	for (auto it = fns.begin(); it != fns.end(); ++it) {
		_generateFunctionCode(stream, *it);
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
		throwSyntaxError("Expected " + LexicalToken::lexemName(type) + ", got " + _currentLexem->toString());
		return false;
	}
	_getNextLexem();
	return true;
}

unsigned int Translator::ArgList(const Scope context)
{

	if (_currentLexem->type() != LexemType::opinc && _currentLexem->type() != LexemType::lpar &&
		_currentLexem->type() != LexemType::opnot && _currentLexem->type() != LexemType::num
		&& _currentLexem->type() != LexemType::id && _currentLexem->type() != LexemType::chr) {
		return 0;
	}

	std::shared_ptr<RValue> p = E(context);

	if (!p) {
		throwSyntaxError("Unknown param format");
	}

	unsigned int m = ArgList_(context);

	generateAtom(std::make_unique<ParamAtom>(p, _paramsList), context);

	return m + 1;
}

unsigned int Translator::ArgList_(const Scope context)
{
	if (_currentLexem->type() == LexemType::comma) {
		_getNextLexem();

		std::shared_ptr<RValue> p = E(context);

		if (!p) {
			throwSyntaxError("Unknown param format");
		}

		unsigned int m = ArgList_(context);

		generateAtom(std::make_unique<ParamAtom>(p, _paramsList), context);

		return m + 1;
	}

	return 0;
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

		std::shared_ptr<MemoryOperand> q = _symbolTable.checkVar(context, _currentLexem->str()); // @TODO: replace with checkVar

		generateAtom(std::make_unique<SimpleBinaryOpAtom>("ADD", q, std::make_shared<NumberOperand>(1), q), context);

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
		_getNextLexem();

		unsigned int n = ArgList(context);

		_takeTerm(LexemType::rpar);

		std::shared_ptr<MemoryOperand> s = _symbolTable.checkFunc(p, n);

		if (!s) {
			throwSyntaxError("Undefined function with name " + p);
		}

		std::shared_ptr<MemoryOperand> r = _symbolTable.alloc(context);

		generateAtom(std::make_unique<CallAtom>(s, r, _symbolTable, _paramsList), context);
		return r;
	}
	else if (_currentLexem->type() == LexemType::opinc) {
		_getNextLexem();

		std::shared_ptr<MemoryOperand> s = _symbolTable.checkVar(context, p); // @Todo:: replace with checkVar
		std::shared_ptr<MemoryOperand> r = _symbolTable.alloc(context);

		generateAtom(std::make_unique<UnaryOpAtom>("MOV", s, r), context);
		generateAtom(std::make_unique<SimpleBinaryOpAtom>("ADD", s, std::make_shared<NumberOperand>(1), s), context);

		return r;
	}
	else if (_currentLexem->type() == LexemType::lbracket) {
		_getNextLexem();

		std::shared_ptr<RValue> key = E(context);

		if (!key) {
			throwSyntaxError("Can't parse index");
		}

		_takeTerm(LexemType::rbracket);

		std::shared_ptr<MemoryOperand> arr = _symbolTable.checkArray(context, p);
		if (!arr) {
			throwSyntaxError(p + " is not an array.");
		}

		return std::make_shared<ArrayElementOperand>(arr->index(), key, &_symbolTable);
	}

	return _symbolTable.checkVar(context, p); // @TODO: replace with checkVar
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

		generateAtom(std::make_unique<FnBinaryOpAtom>("MUL", p, r, s), context);

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

		generateAtom(std::make_unique<SimpleBinaryOpAtom>("ADD", p, r, s), context);

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

		generateAtom(std::make_unique<SimpleBinaryOpAtom>("SUB", p, r, s), context);

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
			generateAtom(std::make_unique<SimpleConditionalJumpAtom>("EQ", p, r, l), context);
		}
		else if (currentLexem == LexemType::opne) {
			generateAtom(std::make_unique<SimpleConditionalJumpAtom>("NE", p, r, l), context);
		}
		else if (currentLexem == LexemType::opgt) {
			generateAtom(std::make_unique<SimpleConditionalJumpAtom>("GT", p, r, l), context);
		}
		else if (currentLexem == LexemType::oplt) {
			generateAtom(std::make_unique<SimpleConditionalJumpAtom>("LT", p, r, l), context);
		}
		else if (currentLexem == LexemType::ople) {
			generateAtom(std::make_unique<ComplexConditinalJumpAtom>("LE", p, r, l), context);
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

		generateAtom(std::make_unique<SimpleBinaryOpAtom>("AND", p, r, s), context);

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

		generateAtom(std::make_unique<SimpleBinaryOpAtom>("OR", p, r, s), context);

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

		generateAtom(std::make_unique<RetAtom>(std::make_shared<NumberOperand>(0), newContext, _symbolTable), newContext);
	}
	else if (_currentLexem->type() == LexemType::opassign) {
		_getNextLexem();

		int val = _currentLexem->value();

		_takeTerm(LexemType::num);

		_symbolTable.insertVar(q, context, p, val);

		DeclVarList_(context, p);

		_takeTerm(LexemType::semicolon);
	}
	else if (_currentLexem->type() == LexemType::lbracket) {
		_getNextLexem();

		int val = _currentLexem->value();

		if (val <= 0) {
			throwSyntaxError("Array size must be greater than 0");
		}

		_takeTerm(LexemType::num);
		_takeTerm(LexemType::rbracket);

		_symbolTable.insertArray(q, context, p, val);

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
			throwSyntaxError("Int or char expected as init value.");
		}

		_getNextLexem();

		_symbolTable.insertVar(q, context, p, val);
	}
	else if(_currentLexem->type() == LexemType::lbracket) {
		_getNextLexem();

		int val = _currentLexem->value();

		if (val <= 0) {
			throwSyntaxError("Array size must be greater than 0");
		}

		_takeTerm(LexemType::num);
		_takeTerm(LexemType::rbracket);

		_symbolTable.insertArray(q, context, p, val);
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

	// If operator outside function, throw error
	if ((type == LexemType::id || type == LexemType::kwwhile || type == LexemType::kwfor ||
		type == LexemType::kwif || type == LexemType::kwswitch || type == LexemType::kwin ||
		type == LexemType::kwout || type == LexemType::semicolon || type == LexemType::lbrace ||
		type == LexemType::kwreturn) && context == SymbolTable::GLOBAL_SCOPE) {
		throwSyntaxError("Operator can't be defined outside function");
	}

	if (type == LexemType::kwchar || type == LexemType::kwint) {
		DeclareStmt(context);
	}
	else if (type == LexemType::id) {
		AssignOrCallOp(context);
	}
	else if (type == LexemType::kwwhile) {
		WhileOp(context);
	}
	else if (type == LexemType::kwfor) {
		ForOp(context);
	}
	else if (type == LexemType::kwif) {
		IfOp(context);
	}
	else if (type == LexemType::kwswitch) {
		SwitchOp(context);
	}
	else if (type == LexemType::kwin) {
		IOp(context);
	}
	else if (type == LexemType::kwout) {
		OOp(context);
	}
	else if (type == LexemType::lbrace) {
		_getNextLexem();
		StmtList(context);
		_takeTerm(LexemType::rbrace);
	}
	else if (type == LexemType::kwreturn) {
		_getNextLexem();
		std::shared_ptr<RValue> p = E(context);

		if (!p) {
			throwSyntaxError("Can't parse return value");
		}

		generateAtom(std::make_unique<RetAtom>(p, context, _symbolTable), context);
		_takeTerm(LexemType::semicolon);
	}
	else if (type == LexemType::semicolon) {
		_getNextLexem();
	}
	else {

		throwSyntaxError("Forbidden lexem"); //@TODO: CHANGE 
	}
}

void Translator::AssignOrCallOp(const Scope context)
{
	AssignOrCall(context);
	_takeTerm(LexemType::semicolon);
}

void Translator::AssignOrCall(const Scope context)
{
	const std::string name = _currentLexem->str();
	_takeTerm(LexemType::id);
	AssignOrCall_(context, name);
}

void Translator::AssignOrCall_(const Scope context, const std::string & p)
{
	if (_currentLexem->type() == LexemType::opassign) {
		_getNextLexem();

		std::shared_ptr<RValue> q = E(context);
		std::shared_ptr<MemoryOperand> r = _symbolTable.checkVar(context, p);
		generateAtom(std::make_unique<UnaryOpAtom>("MOV", q, r), context);
	}
	else if (_currentLexem->type() == LexemType::lbracket) {
		_getNextLexem();

		std::shared_ptr<RValue> index = E(context);

		if (!index) {
			throwSyntaxError("Can't parse array key.");
		}

		_takeTerm(LexemType::rbracket);

		// Check is array
		std::shared_ptr<MemoryOperand> arr = _symbolTable.checkArray(context, p);
		if (!arr) {
			throwSyntaxError(p + " is not array.");
		}

		_takeTerm(LexemType::opassign);

		std::shared_ptr<RValue> value = E(context);
		if (!value) {
			throwSyntaxError("Can't parse value of assignment");
		}

		generateAtom(std::make_unique<UnaryOpAtom>("MOV", value, std::make_shared<ArrayElementOperand>(arr->index(), index, &_symbolTable)), context);

	}
	else if (_currentLexem->type() == LexemType::lpar) {
		_getNextLexem();

		unsigned int n = ArgList(context);
		_takeTerm(LexemType::rpar);

		std::shared_ptr<MemoryOperand> q = _symbolTable.checkFunc(p, n);

		if (!q) {
			throwSyntaxError("Function with name '" + p + "' and len=" + std::to_string(n) + " is not defined");
		}

		std::shared_ptr<MemoryOperand> r = _symbolTable.alloc(context);

		generateAtom(std::make_unique<CallAtom>(q, r, _symbolTable, _paramsList), context);

	}
	else {
		throwSyntaxError("Unexpected lexem, expected ( or =");
	}
}

void Translator::WhileOp(const Scope context)
{
	_takeTerm(LexemType::kwwhile);

	std::shared_ptr<LabelOperand> l1 = newLabel();

	generateAtom(std::make_unique<LabelAtom>(l1), context);

	_takeTerm(LexemType::lpar);

	std::shared_ptr<RValue> p = E(context);
	if (!p) {
		throwSyntaxError("Can't parse while condition");
	}

	_takeTerm(LexemType::rpar);

	std::shared_ptr<LabelOperand> l2 = newLabel();
	generateAtom(std::make_unique<SimpleConditionalJumpAtom>("EQ", p, std::make_shared<NumberOperand>(0), l2), context);

	Stmt(context);

	generateAtom(std::make_unique<JumpAtom>(l1), context);
	generateAtom(std::make_unique<LabelAtom>(l2), context);

}

void Translator::ForOp(const Scope context)
{
	_takeTerm(LexemType::kwfor);
	_takeTerm(LexemType::lpar);

	ForInit(context);

	_takeTerm(LexemType::semicolon);

	std::shared_ptr<LabelOperand> l1 = newLabel();
	generateAtom(std::make_unique<LabelAtom>(l1), context);

	std::shared_ptr<RValue> p = ForExp(context);
	if (!p) {
		throwSyntaxError("Can't parse for condition. ");
	}

	_takeTerm(LexemType::semicolon);

	std::shared_ptr<LabelOperand> l2 = newLabel();
	std::shared_ptr<LabelOperand> l3 = newLabel();
	std::shared_ptr<LabelOperand> l4 = newLabel();

	generateAtom(std::make_unique<SimpleConditionalJumpAtom>("EQ", p, std::make_shared<NumberOperand>(0), l4), context);
	generateAtom(std::make_unique<JumpAtom>(l3), context);
	generateAtom(std::make_unique<LabelAtom>(l2), context);

	ForLoop(context);
	generateAtom(std::make_unique<JumpAtom>(l1), context);

	_takeTerm(LexemType::rpar);

	generateAtom(std::make_unique<LabelAtom>(l3), context);

	Stmt(context);

	generateAtom(std::make_unique<JumpAtom>(l2), context);
	generateAtom(std::make_unique<LabelAtom>(l4), context);
}

void Translator::ForInit(const Scope context)
{
	if (_currentLexem->type() == LexemType::id) {
		AssignOrCall(context);
	}
}

std::shared_ptr<RValue> Translator::ForExp(const Scope context)
{
	if (_currentLexem->type() == LexemType::opinc || _currentLexem->type() == LexemType::lpar || _currentLexem->type() == LexemType::opnot
		|| _currentLexem->type() == LexemType::num || _currentLexem->type() == LexemType::id || _currentLexem->type() == LexemType::chr) {
		return E(context);
	}
	return std::make_shared<NumberOperand>(1);
}

void Translator::ForLoop(const Scope context)
{
	if (_currentLexem->type() == LexemType::id) {
		AssignOrCall(context);
	}
	else if (_currentLexem->type() == LexemType::opinc) {
		_getNextLexem();

		const std::string name = _currentLexem->str();
		_takeTerm(LexemType::id);

		std::shared_ptr<MemoryOperand> p = _symbolTable.checkVar(context, name);

		generateAtom(std::make_unique<SimpleBinaryOpAtom>("ADD", p, std::make_shared<NumberOperand>(1), p), context);
	}
}

void Translator::IfOp(const Scope context)
{
	_takeTerm(LexemType::kwif);
	_takeTerm(LexemType::lpar);

	std::shared_ptr<RValue> p = E(context);

	if (!p) {
		throwSyntaxError("Can't parse if condition.");
	}

	_takeTerm(LexemType::rpar);

	std::shared_ptr<LabelOperand> l1 = newLabel();

	generateAtom(std::make_unique<SimpleConditionalJumpAtom>("EQ", p, std::make_shared<NumberOperand>(0), l1), context);

	Stmt(context);
	std::shared_ptr<LabelOperand> l2 = newLabel();
	generateAtom(std::make_unique<JumpAtom>(l2), context);
	generateAtom(std::make_unique<LabelAtom>(l1), context);

	ElsePart(context);

	generateAtom(std::make_unique<LabelAtom>(l2), context);
}

void Translator::ElsePart(const Scope context)
{
	if (_currentLexem->type() == LexemType::kwelse) {
		_getNextLexem();
		Stmt(context);
	}
}

void Translator::SwitchOp(const Scope context)
{
	_takeTerm(LexemType::kwswitch);
	_takeTerm(LexemType::lpar);

	std::shared_ptr<RValue> p = E(context);
	if (!p) {
		throwSyntaxError("Can't parse switch expression");
	}

	_takeTerm(LexemType::rpar);
	_takeTerm(LexemType::lbrace);

	std::shared_ptr<LabelOperand> end = newLabel();
	Cases(context, p, end);

	_takeTerm(LexemType::rbrace);
	generateAtom(std::make_unique<LabelAtom>(end), context);
}

void Translator::Cases(const Scope context, std::shared_ptr<RValue> p, std::shared_ptr<LabelOperand> end)
{
	std::shared_ptr<LabelOperand> def1 = ACase(context, p, end);
	Cases_(context, p, end, def1);
}

void Translator::Cases_(const Scope context, std::shared_ptr<RValue> p, std::shared_ptr<LabelOperand> end, std::shared_ptr<LabelOperand> def)
{
	if (_currentLexem->type() == LexemType::kwcase || _currentLexem->type() == LexemType::kwdefault) {
		std::shared_ptr<LabelOperand> def1 = ACase(context, p, end);
		
		if (def != nullptr && def1 != nullptr) {
			throwSyntaxError("There can't be more than ONE default section in case.");
		}
		
		std::shared_ptr<LabelOperand> def2 = (def != nullptr) ? def : def1;

		Cases_(context, p, end, def2);
	}
	else {
		std::shared_ptr<LabelOperand> q = end;
		if (def != nullptr) {
			q = def;
		}

		generateAtom(std::make_unique<JumpAtom>(q), context);
	}
}

std::shared_ptr<LabelOperand> Translator::ACase(const Scope context, std::shared_ptr<RValue> p, std::shared_ptr<LabelOperand> end)
{
	if (_currentLexem->type() == LexemType::kwcase) {
		_getNextLexem();
		int val = _currentLexem->value();
		_takeTerm(LexemType::num);

		std::shared_ptr<LabelOperand> next = newLabel();
		generateAtom(std::make_unique<SimpleConditionalJumpAtom>("NE", p, std::make_shared<NumberOperand>(val), next), context);

		_takeTerm(LexemType::colon);
		Stmt(context);

		generateAtom(std::make_unique<JumpAtom>(end), context);
		generateAtom(std::make_unique<LabelAtom>(next), context);

		return nullptr;
	}
	else if (_currentLexem->type() == LexemType::kwdefault) {
		_getNextLexem();
		_takeTerm(LexemType::colon);

		std::shared_ptr<LabelOperand> next = newLabel();
		std::shared_ptr<LabelOperand> def = newLabel();
		generateAtom(std::make_unique<JumpAtom>(next), context);
		generateAtom(std::make_unique<LabelAtom>(def), context);

		Stmt(context);

		generateAtom(std::make_unique<JumpAtom>(end), context);
		generateAtom(std::make_unique<LabelAtom>(next), context);

		return def;
	}
	else {
		throwSyntaxError("Expected case or default. ");
		return nullptr;
	}
}

void Translator::IOp(const Scope context)
{
	_takeTerm(LexemType::kwin);

	const std::string name = _currentLexem->str();
	_takeTerm(LexemType::id);
	_takeTerm(LexemType::semicolon);

	std::shared_ptr<MemoryOperand> p = _symbolTable.checkVar(context, name);

	generateAtom(std::make_unique<InAtom>(p), context);
}

void Translator::OOp(const Scope context)
{
	_takeTerm(LexemType::kwout);
	OOp_(context);
	_takeTerm(LexemType::semicolon);
}

void Translator::OOp_(const Scope context)
{
	if (_currentLexem->type() == LexemType::str) {
		const std::string s = _currentLexem->str();
		_takeTerm(LexemType::str);

		generateAtom(std::make_unique<OutAtom>(_stringTable.insert(s)), context);
	}
	else {
		std::shared_ptr<RValue> p = E(context);
		if (!p) {
			throwSyntaxError("Can't parse out value");
		}

		generateAtom(std::make_unique<OutAtom>(p), context);
	}
}

void Translator::_generateProlog(std::ostream & stream) const
{
	stream << "ORG 0" << std::endl;
	stream << "LXI H, 0" << std::endl;
	stream << "SPHL" << std::endl;
	stream << "CALL main" << std::endl;
	stream << "END" << std::endl;

	stream << "@MULT:" << std::endl << "; code for mult lib" << std::endl;
	stream << "@PRINT:" << std::endl << "; code for mult lib" << std::endl;
}

void Translator::_generateFunctionCode(std::ostream & stream, unsigned int function) const
{
	const SymbolTable::TableRecord* record = &_symbolTable[function];

	stream << record->name << ": ";

	stream << "LXI B, 0" << std::endl;
	for (unsigned int i = 0; i < _symbolTable.getLocalsCount(function); ++i) {
		stream << "PUSH B" << std::endl;
	}

	for (auto it = _atoms.at(function).begin(); it != _atoms.at(function).end(); ++it) {
		
		(*it)->generate(stream);
	}

}
