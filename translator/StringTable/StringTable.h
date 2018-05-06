#pragma once
#include <string>
#include <vector>
#include <memory>
#include "..\Operand\Operand.h"

// Stores info about all string entities
class StringTable {
public:
	// Inserts new string to the table. Returns index of inserted string (or existing)
	std::shared_ptr<StringOperand> insert(const std::string& str);

	const std::string& operator[](const int index) const;
	friend std::ostream& operator<<(std::ostream& stream, const StringTable& table);
private:
	// Stores table records
	std::vector<std::string> _strings;
};

std::ostream& operator<<(std::ostream& stream, const StringTable& table);