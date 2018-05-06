#include "SymbolTable.h"

std::shared_ptr<MemoryOperand> SymbolTable::insert(const std::string & name)
{
	// Check if record exists in table
	for (unsigned int i = 0; i < _records.size(); ++i) {
		if (_records[i].name == name) {
			return std::make_shared<MemoryOperand>(i, this);
		}
	}

	// Record not found, insert
	TableRecord record;
	record.name = name;

	_records.push_back(record);
	return std::make_shared<MemoryOperand>(_records.size() - 1, this);
}

std::shared_ptr<MemoryOperand> SymbolTable::alloc()
{
	_records.push_back(TableRecord());
	return std::make_shared<MemoryOperand>(_records.size() - 1, this);
}

const SymbolTable::TableRecord & SymbolTable::operator[](const int index) const
{
	return _records[index];
}

bool SymbolTable::TableRecord::operator==(const TableRecord & other)
{
	return name == other.name;
}

SymbolTable::TableRecord::TableRecord(std::string & _name)
{
	name = _name;
}

SymbolTable::TableRecord::TableRecord(std::string _name)
{
	name = _name;
}
