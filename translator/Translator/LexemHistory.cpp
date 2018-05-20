#include "LexemHistory.h"

LexemHistory::LexemHistory(const unsigned int size) :_size(size)
{
}

void LexemHistory::push(LexicalToken _token)
{
	if (_list.size() >= _size) {
		_list.pop_front();
	}

	_list.push_back(_token);
}

const LexicalToken & LexemHistory::get(unsigned int index) const
{
	return _list[index];
}

const std::vector<LexicalToken> LexemHistory::getAll() const
{
	return std::vector<LexicalToken>(_list.begin(), _list.end());
}
