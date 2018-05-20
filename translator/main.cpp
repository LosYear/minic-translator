#include <iostream>
#include "LexicalAnalyzer\Scanner.h"
#include "Translator\Translator.h"

int main() {
	std::istringstream stream("(a + b + c");
	Translator translator(stream);

	bool t = translator.translate();

	while (true) {};
	return 0;
}