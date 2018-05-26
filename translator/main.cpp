#include <iostream>
#include "LexicalAnalyzer\Scanner.h"
#include "Translator\Translator.h"

int main() {
	std::istringstream stream("func1()");
	Translator translator(stream);
	translator.insertSymbolTableFunc("func1", SymbolTable::TableRecord::RecordType::integer, 0);

	auto result = translator.translateExpresssion();

	while (true) {};
	return 0;
}