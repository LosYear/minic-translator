#pragma once
#include <memory>
#include <vector>
#include <iostream>
#include "..\Atom\Atom.h"
#include "..\StringTable\StringTable.h"
#include "..\SymbolTable\SymbolTable.h"
#include "..\LexicalAnalyzer\Scanner.h"

class Translator {
public:
	Translator(std::istream& stream);

	// Prints atoms list to a stream
	void printAtoms(std::ostream& stream) const;

	// Adds new atom to list of atoms
	void generateAtom(std::unique_ptr<Atom> atom);

	// Generates new label
	std::shared_ptr<LabelOperand> newLabel();

	// Throws syntax error
	void throwSyntaxError(const std::string& text) const;

	// Throws lexical error
	void throwLexicalError(const std::string& text) const;

	// Runs translation
	bool translate();
private:
	std::vector<std::unique_ptr<Atom>> _atoms;
	StringTable _stringTable;
	SymbolTable _symbolTable;
	LexicalScanner _lexicalAnalyzer;
	std::unique_ptr<LexicalToken> _currentLexem;
	unsigned int _currentLabelId;

	// Gets next token and writes it to _currentLexem
	LexicalToken _getNextLexem();

	// Checks current token and gets next
	bool _takeTerm(LexemType type);

	// Recursive descent rules

public:
	std::shared_ptr<RValue> E1();
	std::shared_ptr<MemoryOperand> E1_(const std::string& p);
};