#pragma once
#include <memory>
#include <vector>
#include <map>
#include <iostream>
#include "..\Atom\Atom.h"
#include "..\StringTable\StringTable.h"
#include "..\SymbolTable\SymbolTable.h"
#include "..\LexicalAnalyzer\Scanner.h"
#include "LexemHistory.h"

class Translator {
public:
	Translator(std::istream& stream, std::ostream& errStream = std::cerr);

	// Prints atoms list to a stream
	void printAtoms(std::ostream& stream, const unsigned int width = 10) const;

	// Prints symbol table to a stream
	void printSymbolTable(std::ostream& stream) const;

	// Adds new atom to list of atoms
	void generateAtom(std::unique_ptr<Atom> atom, Scope scope);

	// Inserts record to symbol table
	std::shared_ptr<MemoryOperand> insertSymbolTableVar(const std::string& name, const Scope scope,
		const SymbolTable::TableRecord::RecordType type, const unsigned int init = 0);
	std::shared_ptr<MemoryOperand> insertSymbolTableFunc(const std::string& name, const SymbolTable::TableRecord::RecordType type, const int len);

	// Generates new label
	std::shared_ptr<LabelOperand> newLabel();

	// Throws syntax error
	void throwSyntaxError(const std::string& text) const;

	// Throws lexical error
	void throwLexicalError(const std::string& text) const;

	// Runs translation
	bool translate();

	// Translates single expression
	std::shared_ptr<RValue> translateExpresssion();
	bool translateExpression(int);
private:
	std::map<Scope, std::vector<std::unique_ptr<Atom>>> _atoms;
	StringTable _stringTable;
	SymbolTable _symbolTable;
	LexicalScanner _lexicalAnalyzer;
	std::unique_ptr<LexicalToken> _currentLexem;
	unsigned int _currentLabelId;

	// History of last 3 lexems
	LexemHistory _lexemHistory;

	// Error stream
	std::ostream& _errStream;

	// Gets next token and writes it to _currentLexem
	LexicalToken _getNextLexem();

	// Checks current token and gets next
	bool _takeTerm(LexemType type);

	// Recursive descent rules of expressions
	unsigned int ArgList(const Scope context);
	unsigned int ArgList_(const Scope context);

	std::shared_ptr<RValue> E1(const Scope context);
	std::shared_ptr<MemoryOperand> E1_(const Scope context, const std::string& p);

	std::shared_ptr<RValue> E2(const Scope context);

	std::shared_ptr<RValue> E3(const Scope context);
	std::shared_ptr<RValue> E3_(const Scope context, std::shared_ptr<RValue> p);

	std::shared_ptr<RValue> E4(const Scope context);
	std::shared_ptr<RValue> E4_(const Scope context, std::shared_ptr<RValue> p);

	std::shared_ptr<RValue> E5(const Scope context);
	std::shared_ptr<RValue> E5_(const Scope context, std::shared_ptr<RValue> p);

	std::shared_ptr<RValue> E6(const Scope context);
	std::shared_ptr<RValue> E6_(const Scope context, std::shared_ptr<RValue> p);

	std::shared_ptr<RValue> E7(const Scope context);
	std::shared_ptr<RValue> E7_(const Scope context, std::shared_ptr<RValue> p);

	std::shared_ptr<RValue> E(const Scope context);

	// Recursive descent rules of miniC
	void DeclareStmt(const Scope context);
	void DeclareStmt_(const Scope context, SymbolTable::TableRecord::RecordType p, const std::string& q);

	SymbolTable::TableRecord::RecordType Type();

	void DeclVarList_(const Scope context, SymbolTable::TableRecord::RecordType p);

	void InitVar(const Scope context, SymbolTable::TableRecord::RecordType p, const std::string& q);

	unsigned int ParamList(const Scope context);
	unsigned int ParamList_(const Scope context);

	void StmtList(const Scope context);
	void Stmt(const Scope context);

	void AssignOrCallOp(const Scope context);
	void AssignOrCall(const Scope context);
	void AssignOrCall_(const Scope context, const std::string& p);

	void WhileOp(const Scope context);
};