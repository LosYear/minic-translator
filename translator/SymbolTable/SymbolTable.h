#pragma once
#include <vector>

// Stores info about all symbols in code
class SymbolTable {
public:
	// Inserts new record to the table. Returns index of inserted record (or existing)
	int insert(const std::string& name);

	// Single element of table
	struct TableRecord {
		TableRecord(std::string& _name);
		TableRecord(std::string _name);
		TableRecord() {};

		std::string name;

		bool operator==(const TableRecord& other);
	};

	const TableRecord& operator[](const int index) const;
private:
	std::vector<TableRecord> _records;
};