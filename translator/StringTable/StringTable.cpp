#include "StringTable.h"

std::shared_ptr<StringOperand> StringTable::insert(const std::string& str)
{
	// Check if string exists in table
	for (unsigned int i = 0; i < _strings.size(); ++i) {
		if (_strings[i] == str) {
			return std::make_shared<StringOperand>(i, this);
		}
	}

	// String not found, insert
	_strings.push_back(str);
	return std::make_shared<StringOperand>(_strings.size() - 1, this);
}

const std::string& StringTable::operator[](const int index) const {
	return _strings[index];
}

std::ostream& operator<<(std::ostream& stream, const StringTable& table) {
	for (unsigned int i = 0; i < table._strings.size(); ++i) {
		stream << i << " " << table._strings[i];

		if (i != table._strings.size() - 1) {
			stream << std::endl;
		}
	}

	return stream;
}