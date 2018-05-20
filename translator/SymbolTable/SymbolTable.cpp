#include <iomanip>
#include "SymbolTable.h"

std::shared_ptr<MemoryOperand> SymbolTable::insert(const std::string & name, TableRecord::RecordKind kind, TableRecord::RecordType type, int len, int init, Scope scope, int offset)
{
	// Record not found, insert
	TableRecord record(name, kind, type, len, init, scope, offset);

	// Check if record exists in table
	for (unsigned int i = 0; i < _records.size(); ++i) {
		if (_records[i] == record) {
			return std::make_shared<MemoryOperand>(i, this);
		}
	}

	_records.push_back(record);
	return std::make_shared<MemoryOperand>(_records.size() - 1, this);
}

std::shared_ptr<MemoryOperand> SymbolTable::alloc()
{
	_records.push_back(TableRecord("[tmp" + std::to_string(_records.size()) + "]"));
	return std::make_shared<MemoryOperand>(_records.size() - 1, this);
}

const SymbolTable::TableRecord & SymbolTable::operator[](const int index) const
{
	return _records[index];
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