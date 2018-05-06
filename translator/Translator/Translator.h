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
private:
	std::vector<std::unique_ptr<Atom>> _atoms;
	StringTable _stringTable;
	SymbolTable _symbolTable;
	LexicalScanner _lexicalAnalyzer;
	LexicalToken _currentLexem;
	unsigned int _currentLabelId;
};