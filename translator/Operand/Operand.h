#pragma once
#include <string>
#include <memory>

class SymbolTable;
class StringTable;

// Base class for all operands
class Operand {
	// Represents operand as string
public:
	virtual std::string toString(bool expanded = false) const = 0;
};


// Base class for all math operands
class RValue : public Operand {
public: 
	// Generates i8080 code to load given operand to A reg
	virtual void load(std::ostream& stream) const = 0;
};


// Operands stored in memory
class MemoryOperand : public RValue {
public:
	MemoryOperand(const int index, const SymbolTable* symbolTable);
	std::string toString(bool expanded = false) const;

	const int index() const;

	bool operator==(MemoryOperand& other);

	// Generates i8080 code to save A reg to given place
	void save(std::ostream& stream) const;

	void load(std::ostream& stream) const;
protected:
	const int _index;
	const SymbolTable* _symbolTable;
};

// Array element
class ArrayElementOperand : public MemoryOperand {
public:
	ArrayElementOperand(const int index, std::shared_ptr<RValue> elementIndex, const SymbolTable* symbolTable);
	std::string toString(bool expanded = false) const;

	bool operator==(ArrayElementOperand& other);

	const std::shared_ptr<RValue> elementIndex() const;

	// Generates i8080 code to save A reg to given place
	void save(std::ostream& stream) const { stream; };

	void load(std::ostream& stream) const { stream; };
protected:
	const std::shared_ptr<RValue>_elementIndex;
};


// Integer operands
class NumberOperand : public RValue {
public:
	NumberOperand(const int value);
	std::string toString(bool expanded = false) const;

	void load(std::ostream& stream) const;
private:
	const int _value;
};


// Operand from StringTable
class StringOperand : public Operand {
public:
	StringOperand(const int index, const StringTable* stringTable);
	std::string toString(bool expanded = false) const;
	const int index() const;

	bool operator==(StringOperand& other);
private:
	const int _index;
	const StringTable* _stringTable;
};


// Labels
class LabelOperand : public Operand {
public:
	LabelOperand(const int labelID);
	std::string toString(bool expanded = false) const;
	int id() const;
private:
	int _id;
};