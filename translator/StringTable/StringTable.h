#pragma once
#include <string>
#include <vector>

class StringTable {
public:
	// Inserts new string to the table. Returns index of inserted string (or existing)
	int insert(const std::string str);

	const std::string& operator[](const int index) const;
	friend std::ostream& operator<<(std::ostream& stream, const StringTable& table);
private:
	// Stores table records
	std::vector<std::string> _strings;
};

std::ostream& operator<<(std::ostream& stream, const StringTable& table);