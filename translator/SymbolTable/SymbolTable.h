#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include <string>
#include "..\Operand\Operand.h"

typedef int Scope;

// Stores info about all symbols in code
class SymbolTable {
public:
	// Global scope const
	static const Scope GLOBAL_SCOPE = -1;

	// Single element of table
	struct TableRecord {
		enum class RecordKind { unknown, var, func };
		enum class RecordType { unknown, integer, chr };

		TableRecord(std::string& _name,
			RecordKind _kind = RecordKind::unknown,
			RecordType _type = RecordType::unknown,
			int _len = -1,
			int _init = 0,
			Scope _scope = SymbolTable::GLOBAL_SCOPE,
			int _offset = -1);

		TableRecord(std::string _name,
			RecordKind _kind = RecordKind::unknown,
			RecordType _type = RecordType::unknown,
			int _len = -1,
			int _init = 0,
			Scope _scope = SymbolTable::GLOBAL_SCOPE,
			int _offset = -1);

		TableRecord() {};

		std::string name;
		RecordKind kind;
		RecordType type;
		int len;
		int init;
		Scope scope;
		int offset;

		bool operator==(const TableRecord& other);
	};

	// Inserts new record to the table. Returns index of inserted record (or existing)
	std::shared_ptr<MemoryOperand> insert(const std::string& name, TableRecord::RecordKind kind = TableRecord::RecordKind::unknown,
		TableRecord::RecordType type = TableRecord::RecordType::unknown,
		int len = -1,
		int init = 0,
		Scope scope = SymbolTable::GLOBAL_SCOPE,
		int offset = -1);

	// Allocate record for temporary variable
	std::shared_ptr<MemoryOperand> alloc();

	const TableRecord& operator[](const int index) const;
	friend std::ostream& operator<<(std::ostream& stream, const SymbolTable& table);
private:
	std::vector<TableRecord> _records;
};

std::ostream& operator<<(std::ostream& stream, const SymbolTable& table);