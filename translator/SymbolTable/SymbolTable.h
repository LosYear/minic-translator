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

	// Inserts new variable into the table. If var with given name and scope exists, returns nullptr
	std::shared_ptr<MemoryOperand> insertVar(const std::string& name, const Scope scope,
		const TableRecord::RecordType type, const unsigned int init = 0);

	// Inserts new function into the table. If var or function with given name exists, returns nullptr
	std::shared_ptr<MemoryOperand> insertFunc(const std::string& name, const TableRecord::RecordType type, const int len);

	// Find variable in given scope. If there's no var, returns nullptr
	std::shared_ptr<MemoryOperand> checkVar(const Scope scope, const std::string& name);

	// Checks whether given name is function with given count of arguments
	std::shared_ptr<MemoryOperand> checkFunc(const std::string& name, const int len);

	// Changes args count for function
	bool changeArgsCount(const int index, const int len);

	// Allocate record for temporary variable
	std::shared_ptr<MemoryOperand> alloc(Scope scope);

	// Counts locals and temp variables with given scope
	unsigned int getLocalsCount(const Scope scope) const;

	// Recalculates offset for all symbol table
	void calculateOffset();

	// Returns names all of functions in string table
	std::vector<std::string> functionNames() const;

	const TableRecord& operator[](const int index) const;
	friend std::ostream& operator<<(std::ostream& stream, const SymbolTable& table);
private:
	std::vector<TableRecord> _records;
};

std::ostream& operator<<(std::ostream& stream, const SymbolTable& table);