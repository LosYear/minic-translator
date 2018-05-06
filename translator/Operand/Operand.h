#pragma once
#include <string>

class SymbolTable;
class StringTable;

// Base class for all operands
class Operand {
	// Represents operand as string
	virtual std::string toString() const = 0;
};


// Base class for all math operands
class RValue : public Operand {};


// Operands stored in memory
class MemoryOperand : public RValue {
public:
	MemoryOperand(const int index, const SymbolTable* symbolTable);
	std::string toString() const;
private:
	const int _index;
	const SymbolTable* _symbolTable;
};


// Integer operands
class NumberOperand : public RValue {
public:
	NumberOperand(const int value);
	std::string toString() const;
private:
	const int _value;
};


// Operand from StringTable
class StringOperand : public Operand {
public:
	StringOperand(const int index, const StringTable* stringTable);
	std::string toString() const;
private:
	const int _index;
	const StringTable* _stringTable;
};


// Labels
class LabelOperand : public Operand {
public:
	LabelOperand(const int labelID);
	std::string toString() const;
private:
	int _id;
};