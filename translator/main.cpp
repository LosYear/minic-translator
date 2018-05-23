#include <iostream>
#include "LexicalAnalyzer\Scanner.h"
#include "Translator\Translator.h"

int main() {
	std::istringstream stream("int n(){}");
	Translator translator(stream);

	bool translated = translator.translate();

	while (true) {};
	return 0;
}