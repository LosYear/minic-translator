#include <iomanip>
#include <map>
#include "SymbolTable.h"

std::shared_ptr<MemoryOperand> SymbolTable::insertVar(const std::string & name, const Scope scope, const TableRecord::RecordType type, const unsigned int init)
{
	// Check if record exists in table
	for (unsigned int i = 0; i < _records.size(); ++i) {
		if (_records[i].name == name && _records[i].scope == scope) {
			return nullptr;
		}
	}

	// Record not found, insert
	TableRecord record(name, TableRecord::RecordKind::var, type, -1, init, scope, 0);

	_records.push_back(record);
	return std::make_shared<MemoryOperand>(_records.size() - 1, this);
}

std::shared_ptr<MemoryOperand> SymbolTable::insertArray(const std::string & name, const Scope scope, const TableRecord::RecordType type, const unsigned int len)
{
	// Check if record exists in table
	for (unsigned int i = 0; i < _records.size(); ++i) {
		if (_records[i].name == name && _records[i].scope == scope) {
			return nullptr;
		}
	}

	// Record not found, insert
	TableRecord record(name, TableRecord::RecordKind::array, type, len, 0, scope, 0);

	_records.push_back(record);
	return std::make_shared<MemoryOperand>(_records.size() - 1, this);
}

std::shared_ptr<MemoryOperand> SymbolTable::insertFunc(const std::string & name, const TableRecord::RecordType type, const int len)
{
	// Check if record exists in table
	for (unsigned int i = 0; i < _records.size(); ++i) {
		if (_records[i].name == name && _records[i].scope == SymbolTable::GLOBAL_SCOPE) {
			return nullptr;
		}
	}

	// Record not found, insert
	TableRecord record(name, TableRecord::RecordKind::func, type, len, 0, SymbolTable::GLOBAL_SCOPE, 0);

	_records.push_back(record);
	return std::make_shared<MemoryOperand>(_records.size() - 1, this);
}

std::shared_ptr<MemoryOperand> SymbolTable::checkVar(const Scope scope, const std::string & name)
{
	// Find var in given scope
	int globalScopedValue = -1;

	for (unsigned int i = 0; i < _records.size(); ++i) {
		if (_records[i].name == name && _records[i].scope == scope &&
			_records[i].kind == SymbolTable::TableRecord::RecordKind::var) {
			return std::make_shared<MemoryOperand>(i, this);
		}
		else if (_records[i].name == name && _records[i].scope == SymbolTable::GLOBAL_SCOPE
			&& _records[i].kind == SymbolTable::TableRecord::RecordKind::var) {
			globalScopedValue = i;
		}
	}

	if (globalScopedValue == -1) {
		return nullptr;
	}

	return std::make_shared<MemoryOperand>(globalScopedValue, this);
}

std::shared_ptr<MemoryOperand> SymbolTable::checkFunc(const std::string & name, const int len)
{
	for (unsigned int i = 0; i < _records.size(); ++i) {
		if (_records[i].name == name && _records[i].len == len &&
			_records[i].kind == SymbolTable::TableRecord::RecordKind::func &&
			_records[i].scope == SymbolTable::GLOBAL_SCOPE) {
			return std::make_shared<MemoryOperand>(i, this);
		}
	}

	return nullptr;
}

std::shared_ptr<MemoryOperand> SymbolTable::checkArray(const Scope scope, const std::string & name)
{
	// Find var in given scope
	int globalScopedValue = -1;

	for (unsigned int i = 0; i < _records.size(); ++i) {
		if (_records[i].name == name && _records[i].scope == scope &&
			_records[i].kind == SymbolTable::TableRecord::RecordKind::array) {
			return std::make_shared<MemoryOperand>(i, this);
		}
		else if (_records[i].name == name && _records[i].scope == SymbolTable::GLOBAL_SCOPE
			&& _records[i].kind == SymbolTable::TableRecord::RecordKind::array) {
			globalScopedValue = i;
		}
	}

	if (globalScopedValue == -1) {
		return nullptr;
	}

	return std::make_shared<MemoryOperand>(globalScopedValue, this);
}

bool SymbolTable::changeArgsCount(const int index, const int len)
{
	if (_records[index].kind != SymbolTable::TableRecord::RecordKind::func) {
		return false;
	}

	_records[index].len = len;
	return true;
}

std::shared_ptr<MemoryOperand> SymbolTable::alloc(Scope scope)
{
	_records.push_back(TableRecord("[tmp" + std::to_string(_records.size()) + "]",
		TableRecord::RecordKind::var,
		TableRecord::RecordType::integer,
		-1, 0, scope));
	return std::make_shared<MemoryOperand>(_records.size() - 1, this);
}

unsigned int SymbolTable::getLocalsCount(const Scope scope) const
{
	unsigned int vars = 0;

	for (auto it = _records.begin(); it != _records.end(); ++it) {
		if (it->scope == scope && it->kind == TableRecord::RecordKind::var){
			vars++;
		}
	}

	return vars - _records[scope].len;
}

void SymbolTable::calculateOffset()
{
	std::map<const Scope, unsigned int> counts;
	std::map<const Scope, unsigned int> arraysOffsets;
	unsigned int i = 0;
	for (auto it = _records.begin(); it != _records.end(); ++it, ++i) {

		if (it->kind == TableRecord::RecordKind::var && it->scope != SymbolTable::GLOBAL_SCOPE) {
			unsigned int n = _records[it->scope].len;
			unsigned int m = getLocalsCount(it->scope);
			unsigned int arrays = _getArraysSize(it->scope);

			unsigned int j = 1;

			if (counts.find(it->scope) != counts.end()) {
				j = counts[it->scope] + 1;
			}
			counts[it->scope] = j;

			if (j <= n) {
				it->offset = 2 * (m + n + 1 - j) + 2 * arrays;
			}
			else {

				it->offset = 2 * (m + n - j);
			}
		}
		else if (it->kind == TableRecord::RecordKind::func) {
			unsigned int n = _records[i].len;
			unsigned int m = getLocalsCount(i);
			unsigned int arrays = _getArraysSize(i);
			it->offset = 2 * (m + n + 1) + 2 * arrays;
		}
		else if (it->kind == TableRecord::RecordKind::array && it->scope != SymbolTable::GLOBAL_SCOPE) {
			if (arraysOffsets.find(it->scope) == arraysOffsets.end()) {
				arraysOffsets[it->scope] = getLocalsCount(it->scope) * 2;
			}

			it->offset = arraysOffsets[it->scope];
			arraysOffsets[it->scope] += 2 * it->len;
		}
	}
}

void SymbolTable::generateGlobalsSection(std::ostream & stream) const
{
	for (unsigned int i = 0; i < _records.size(); ++i) {
		if (_records[i].scope == SymbolTable::GLOBAL_SCOPE && _records[i].kind == SymbolTable::TableRecord::RecordKind::var) {
			stream << "var" << i << ": DB " << _records[i].init << std::endl;
		}
	}
}

std::vector<std::string> SymbolTable::functionNames() const
{
	std::vector<std::string> result;

	for (auto it = _records.begin(); it != _records.end(); ++it) {
		if (it->kind == TableRecord::RecordKind::func) {
			result.push_back(it->name);
		}
	}

	return result;
}

std::vector<unsigned int> SymbolTable::functionsIds() const
{
	std::vector<unsigned int> result;

	for (unsigned int i = 0; i < _records.size(); ++i) {
		if (_records[i].kind == TableRecord::RecordKind::func) {
			result.push_back(i);
		}
	}

	return result;
}

const SymbolTable::TableRecord & SymbolTable::operator[](const int index) const
{
	return _records[index];
}

unsigned int SymbolTable::_getArraysSize(const Scope scope)
{
	unsigned int result = 0;

	for (auto it = _records.begin(); it != _records.end(); ++it) {
		if (it->scope == scope && it->kind == SymbolTable::TableRecord::RecordKind::array){
			result += it->len;
		}
	}

	return result;
}

SymbolTable::TableRecord::TableRecord(std::string & _name, RecordKind _kind, RecordType _type, int _len, int _init, Scope _scope, int _offset)
{
	name = _name;
	kind = _kind;
	type = _type;
	len = _len;
	init = _init;
	scope = _scope;
	offset = _offset;
}

SymbolTable::TableRecord::TableRecord(std::string _name, RecordKind _kind, RecordType _type, int _len, int _init, Scope _scope, int _offset)
{
	name = _name;
	kind = _kind;
	type = _type;
	len = _len;
	init = _init;
	scope = _scope;
	offset = _offset;
}

bool SymbolTable::TableRecord::operator==(const TableRecord & other)
{
	return name == other.name
		&& kind == other.kind && type == other.type
		&& len == other.len && init == other.init
		&& scope == other.scope && offset == other.offset;
}

std::ostream & operator<<(std::ostream & stream, const SymbolTable & table)
{
	const unsigned int w = 10;

	stream << std::left << "SYMBOL TABLE:" << std::endl;
	stream << std::string(10, '-') << std::endl;

	// Header
	stream << std::setw(w) << "code" << std::setw(1) << " ";
	stream << std::setw(w) << "name" << std::setw(1) << " ";
	stream << std::setw(w) << "kind" << std::setw(1) << " ";
	stream << std::setw(w) << "type" << std::setw(1) << " ";
	stream << std::setw(w) << "len" << std::setw(1) << " ";
	stream << std::setw(w) << "init" << std::setw(1) << " ";
	stream << std::setw(w) << "scope" << std::setw(1) << " ";
	stream << std::setw(w) << "offset" << std::setw(1) << " " << std::endl;

	// Print rows
	for (unsigned int i = 0; i < table._records.size(); ++i) {
		auto& record = table._records[i];

		stream << std::setw(w) << i << std::setw(1) << " ";
		stream << std::setw(w) << record.name << std::setw(1) << " ";


		stream << std::setw(w);
		switch (record.kind) {
		case SymbolTable::TableRecord::RecordKind::func: stream << "func"; break;
		case SymbolTable::TableRecord::RecordKind::var: stream << "var"; break;
		case SymbolTable::TableRecord::RecordKind::array: stream << "array"; break;
		case SymbolTable::TableRecord::RecordKind::unknown: stream << "unknown"; break;
		}
		stream << std::setw(1) << " ";

		stream << std::setw(w);
		switch (record.type) {
		case SymbolTable::TableRecord::RecordType::unknown: stream << "unknown"; break;
		case SymbolTable::TableRecord::RecordType::chr: stream << "chr"; break;
		case SymbolTable::TableRecord::RecordType::integer: stream << "integer"; break;
		}
		stream << std::setw(1) << " ";

		stream << std::setw(w) << record.len << std::setw(1) << " ";
		stream << std::setw(w) << record.init << std::setw(1) << " ";
		stream << std::setw(w) << record.scope << std::setw(1) << " ";
		stream << std::setw(w) << record.offset << std::setw(1) << " " << std::endl;
	}
	return stream;
}