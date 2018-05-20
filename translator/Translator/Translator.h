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

	// Adds new atom to list of atoms
	void generateAtom(std::unique_ptr<Atom> atom, Scope scope);

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

	// Recursive descent rules
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
};