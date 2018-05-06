#include "Operand.h"
#include "..\StringTable\StringTable.h"
#include "..\SymbolTable\SymbolTable.h"


MemoryOperand::MemoryOperand(const int index, const SymbolTable * symbolTable) : _index(index),
_symbolTable(symbolTable) {}

LabelOperand::LabelOperand(const int labelID) : _id(labelID) {}

StringOperand::StringOperand(const int index, const StringTable * stringTable) : _index(index),
_stringTable(stringTable) {}

NumberOperand::NumberOperand(const int value) : _value(value) {}

std::string MemoryOperand::toString() const
{
	std::string str = "[MemOp, " + std::to_string(_index) + ", " + (*_symbolTable)[_index].name + "]";
	return str;
}

std::string NumberOperand::toString() const
{
	std::string str = "[NumOp, " + std::to_string(_value) + "]";
	return str;
}

std::string StringOperand::toString() const
{
	std::string str = "[StrOp, " + std::to_string(_index) + ", '" + (*_stringTable)[_index] + "']";
	return str;
}

std::string LabelOperand::toString() const
{
	std::string str = "[LblOp, " + std::to_string(_id) + "]";
	return str;
}
