#include <deque>
#include <vector>
#include "..\LexicalAnalyzer\Token.h"
#pragma once

// Stores last n lexems
class LexemHistory {
public:
	// Pushes new lexem and removes one if there's need
	void push(LexicalToken _token);

	// Returns lexem with given index
	const LexicalToken& get(unsigned int index) const;

	// Gets vector of all lexems
	const std::vector<LexicalToken> getAll() const;
private:
	// Max list size
	const unsigned int _size = 3;

	// Container
	std::deque<LexicalToken> _list;
};