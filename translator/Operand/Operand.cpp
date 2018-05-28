#include "Operand.h"
#include "..\StringTable\StringTable.h"
#include "..\SymbolTable\SymbolTable.h"


MemoryOperand::MemoryOperand(const int index, const SymbolTable * symbolTable) : _index(index),
_symbolTable(symbolTable) {}

LabelOperand::LabelOperand(const int labelID) : _id(labelID) {}

StringOperand::StringOperand(const int index, const StringTable * stringTable) : _index(index),
_stringTable(stringTable) {}

NumberOperand::NumberOperand(const int value) : _value(value) {}

std::string MemoryOperand::toString(bool expanded) const
{
	if (!expanded) {
		return std::to_string(_index);
	}

	std::string str = "[MemOp, " + std::to_string(_index) + ", " + (*_symbolTable)[_index].name + "]";
	return str;
}

const int MemoryOperand::index() const
{
	return _index;
}

bool MemoryOperand::operator==(MemoryOperand & other)
{
	return _index == other._index && _symbolTable == other._symbolTable;
}

void MemoryOperand::save(std::ostream & stream) const
{
	if ((*_symbolTable)[_index].scope == SymbolTable::GLOBAL_SCOPE) {
		stream << "STA VAR" << _index << std::endl;
	}
	else {
		unsigned int offset = (*_symbolTable)[_index].offset;

		stream << "LXI H, " << offset << std::endl;
		stream << "DAD SP" << std::endl;
		stream << "MOV M, A" << std::endl;
	}
}

void MemoryOperand::load(std::ostream & stream) const
{
	if ((*_symbolTable)[_index].scope == SymbolTable::GLOBAL_SCOPE) {
		stream << "LDA VAR" << _index << std::endl;
	}
	else {
		unsigned int offset = (*_symbolTable)[_index].offset;

		stream << "LXI H, " << offset << std::endl;
		stream << "DAD SP" << std::endl;

		stream << "MOV A, M" << std::endl;
	}
}

bool StringOperand::operator==(StringOperand & other)
{
	return _index == other._index && _stringTable == other._stringTable;
}

std::string NumberOperand::toString(bool expanded) const
{
	if (!expanded) {
		return "'" + std::to_string(_value) + "'";
	}

	std::string str = "[NumOp, " + std::to_string(_value) + "]";
	return str;
}

void NumberOperand::load(std::ostream & stream) const
{
	stream << "MVI A, " << std::to_string(_value) << std::endl;
}

std::string StringOperand::toString(bool expanded) const
{
	if (!expanded) {
		return "str`" + std::to_string(_index) + "`";
	}

	std::string str = "[StrOp, " + std::to_string(_index) + ", '" + (*_stringTable)[_index] + "']";
	return str;
}

std::string LabelOperand::toString(bool expanded) const
{
	if (!expanded) {
		return "lbl`" + std::to_string(_id) + "`";
	}

	std::string str = "[LblOp, " + std::to_string(_id) + "]";
	return str;
}

int LabelOperand::id() const
{
	return _id;
}
